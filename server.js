/**
 * server.js — Backend que consome o executável C (cozinha_api.exe) de verdade.
 *
 * Arquitetura:
 *   Browser → HTTP → Node.js → stdin → cozinha_api.exe (C)
 *                                     ← stdout (JSON) ←
 */

const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');

const PORT = 3000;
const WEB_DIR = path.join(__dirname, 'interface_web');
const API_EXE = path.join(__dirname, 'cozinha_api.exe');

// ─── Spawn do processo C ──────────────────────────────────────────────────────
let cProcess = null;
let responseBuffer = '';
let pendingResolve = null;

function startCProcess() {
    cProcess = spawn(API_EXE, [], {
        cwd: __dirname,
        stdio: ['pipe', 'pipe', 'pipe']
    });

    cProcess.stdout.setEncoding('utf8');

    cProcess.stdout.on('data', (chunk) => {
        responseBuffer += chunk;
        // Pode vir múltiplas linhas de uma vez, ou parcial
        let newlineIdx;
        while ((newlineIdx = responseBuffer.indexOf('\n')) !== -1) {
            const line = responseBuffer.slice(0, newlineIdx).trim();
            responseBuffer = responseBuffer.slice(newlineIdx + 1);
            if (line && pendingResolve) {
                const resolve = pendingResolve;
                pendingResolve = null;
                resolve(line);
            }
        }
    });

    cProcess.stderr.on('data', (d) => {
        // Mensagens de debug internas do C — logging só aqui
        console.error('[C]', d.toString().trim());
    });

    cProcess.on('close', (code) => {
        console.log(`[C process] encerrado (code ${code}). Reiniciando...`);
        // Rejeita qualquer comando pendente
        if (pendingResolve) {
            pendingResolve = null;
        }
        responseBuffer = '';
        processing = false;
        setTimeout(startCProcess, 500);
    });

    console.log('[C process] cozinha_api.exe iniciado.');
}

// ─── Fila serializada de comandos ─────────────────────────────────────────────
const cmdQueue = [];
let processing = false;

function sendCommand(cmd) {
    return new Promise((resolve, reject) => {
        cmdQueue.push({ cmd, resolve, reject });
        processQueue();
    });
}

function processQueue() {
    if (processing || cmdQueue.length === 0) return;
    if (!cProcess || cProcess.killed) {
        const { reject } = cmdQueue.shift();
        reject(new Error('Processo C não disponível'));
        processQueue();
        return;
    }
    processing = true;

    const { cmd, resolve, reject } = cmdQueue.shift();

    const timeout = setTimeout(() => {
        pendingResolve = null;
        processing = false;
        reject(new Error('Timeout: o processo C nao respondeu'));
        processQueue();
    }, 10000);

    pendingResolve = (line) => {
        clearTimeout(timeout);
        processing = false;
        try {
            resolve(JSON.parse(line));
        } catch (e) {
            reject(new Error('JSON inválido do C: ' + line.substring(0, 200)));
        }
        processQueue();
    };

    cProcess.stdin.write(cmd + '\n');
}

// ─── Servidor HTTP ────────────────────────────────────────────────────────────
const server = http.createServer(async (req, res) => {
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, DELETE, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

    if (req.method === 'OPTIONS') { res.writeHead(204); res.end(); return; }

    // ── API ──────────────────────────────────────────────────────────────────
    if (req.url.startsWith('/api')) {
        let body = '';
        if (req.method === 'POST') {
            await new Promise(r => {
                req.on('data', c => body += c);
                req.on('end', r);
            });
        }

        let result;
        try {
            const url = req.url;
            const method = req.method;

            if (url === '/api/data' && method === 'GET') {
                result = await sendCommand('GET_ALL');

            } else if (url === '/api/catalog' && method === 'POST') {
                const { name, unit } = JSON.parse(body);
                result = await sendCommand(`ADD_CATALOGO ${name}|${unit}`);

            } else if (url.startsWith('/api/catalog/') && method === 'DELETE') {
                const id = url.split('/').pop();
                result = await sendCommand(`DEL_CATALOGO ${id}`);

            } else if (url === '/api/stock' && method === 'POST') {
                const { id, qtd } = JSON.parse(body);
                result = await sendCommand(`ADD_ESTOQUE ${id} ${qtd}`);

            } else if (url.startsWith('/api/stock/') && method === 'DELETE') {
                const id = url.split('/').pop();
                result = await sendCommand(`DEL_ESTOQUE ${id}`);

            } else if (url === '/api/recipe' && method === 'POST') {
                const { name, preparo } = JSON.parse(body);
                result = await sendCommand(`ADD_RECEITA ${name}|${preparo}`);

            } else if (url.match(/^\/api\/recipe\/\d+$/) && method === 'DELETE') {
                const id = url.split('/').pop();
                result = await sendCommand(`DEL_RECEITA ${id}`);

            } else if (url === '/api/recipe/ingredient' && method === 'POST') {
                const { id_receita, id_ingrediente, qtd } = JSON.parse(body);
                result = await sendCommand(`ADD_ING_RECEITA ${id_receita} ${id_ingrediente} ${qtd}`);

            } else if (url === '/api/order' && method === 'POST') {
                const { id } = JSON.parse(body);
                result = await sendCommand(`ADD_PEDIDO ${id}`);

            } else if (url === '/api/order/process' && method === 'POST') {
                result = await sendCommand('PROCESSAR_PEDIDO');

            } else if (url.match(/^\/api\/order\/\d+$/) && method === 'DELETE') {
                const id = url.split('/').pop();
                result = await sendCommand(`DEL_PEDIDO ${id}`);

            } else {
                res.writeHead(404, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ error: 'Rota não encontrada' }));
                return;
            }

            res.writeHead(200, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify(result));

        } catch (e) {
            console.error('[API Error]', e.message);
            res.writeHead(500, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify({ error: e.message }));
        }
        return;
    }

    // ── Arquivos Estáticos ───────────────────────────────────────────────────
    let filePath = path.join(WEB_DIR, req.url === '/' ? 'index.html' : req.url);
    const ext = path.extname(filePath);
    const types = {
        '.html': 'text/html; charset=utf-8',
        '.js': 'text/javascript; charset=utf-8',
        '.css': 'text/css; charset=utf-8'
    };

    fs.readFile(filePath, (err, content) => {
        if (err) { res.writeHead(404); res.end('Not Found'); return; }
        res.writeHead(200, { 'Content-Type': types[ext] || 'text/plain' });
        res.end(content);
    });
});

// ─── Inicialização ────────────────────────────────────────────────────────────
startCProcess();
server.listen(PORT, () => {
    console.log(`\n${'='.repeat(52)}`);
    console.log(`  🚀 Dashboard: http://localhost:${PORT}`);
    console.log(`  🔧 Backend:   Node.js → cozinha_api.exe (C)`);
    console.log(`  📂 Dados:     ./data/*.txt`);
    console.log(`${'='.repeat(52)}\n`);
});

process.on('SIGINT', () => {
    console.log('\nEncerrando...');
    if (cProcess) {
        cProcess.stdin.write('QUIT\n');
        setTimeout(() => { cProcess.kill(); process.exit(0); }, 500);
    } else {
        process.exit(0);
    }
});
