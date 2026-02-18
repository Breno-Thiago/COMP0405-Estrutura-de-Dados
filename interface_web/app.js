/**
 * app.js — Frontend completo integrado com cozinha_api.exe via Node.js
 * Sem polling automático. Atualiza só após ações do usuário.
 */

// ── Estado ────────────────────────────────────────────────────────────────────
let state = { catalog: [], inventory: [], recipes: [], orders: [] };
let currentTab = 'dashboard';

// ── API helper ────────────────────────────────────────────────────────────────
async function api(url, method = 'GET', body = null) {
    const opts = { method, headers: { 'Content-Type': 'application/json' } };
    if (body) opts.body = JSON.stringify(body);
    const res = await fetch(url, opts);
    const json = await res.json();
    return json;
}

async function syncData() {
    try {
        const data = await api('/api/data');
        if (data.error) throw new Error(data.error);
        state = data;
        setSyncStatus(true);
    } catch (e) {
        setSyncStatus(false);
        console.error('Sync error:', e);
    }
    renderCurrentTab();
}

function setSyncStatus(ok) {
    document.getElementById('sync-status').className = 'sync-dot' + (ok ? '' : ' error');
    document.getElementById('sync-label').textContent = ok ? 'Sincronizado com C' : 'Sem conexão';
}

// ── Toast ─────────────────────────────────────────────────────────────────────
function toast(msg, type = 'success', duration = 4000) {
    const el = document.getElementById('toast');
    el.innerHTML = msg;
    el.className = `toast ${type} show`;
    clearTimeout(el._t);
    el._t = setTimeout(() => el.classList.remove('show'), duration);
}

// ── Inicialização ─────────────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
    syncData();
    document.addEventListener('keydown', e => { if (e.key === 'Escape') closeModal(); });
});

function switchTab(tab) {
    currentTab = tab;
    document.querySelectorAll('nav li').forEach(li =>
        li.classList.toggle('active', li.dataset.tab === tab));
    renderCurrentTab();
}

function renderCurrentTab() {
    const r = {
        dashboard: renderDashboard, catalog: renderCatalog,
        inventory: renderInventory, recipes: renderRecipes,
        orders: renderOrders
    };
    (r[currentTab] || renderDashboard)();
}

// ── Helpers ───────────────────────────────────────────────────────────────────
const catItem = id => state.catalog.find(i => i.id === id);
const catName = id => { const i = catItem(id); return i ? i.name : '#' + id; };
const catUnit = id => { const i = catItem(id); return i ? i.unit : ''; };
const val = id => (document.getElementById(id)?.value || '').trim();
const emptyRow = (n, icon, msg) => `<tr><td colspan="${n}"><div class="empty-state"><div class="icon">${icon}</div><p>${msg}</p></div></td></tr>`;

// ── DASHBOARD ─────────────────────────────────────────────────────────────────
function renderDashboard() {
    document.getElementById('main-content').innerHTML = `
        <header class="fade-in"><h1>Dashboard</h1><p>Dados em tempo real do <code>cozinha_api.exe</code></p></header>
        <div class="stats-grid fade-in">
            <div class="stat-card" onclick="switchTab('catalog')" style="cursor:pointer">
                <div class="stat-label">Catálogo</div><div class="stat-value">${state.catalog.length}</div>
            </div>
            <div class="stat-card" onclick="switchTab('inventory')" style="cursor:pointer">
                <div class="stat-label">Estoque</div><div class="stat-value">${state.inventory.length}</div>
            </div>
            <div class="stat-card" onclick="switchTab('recipes')" style="cursor:pointer">
                <div class="stat-label">Receitas</div><div class="stat-value">${state.recipes.length}</div>
            </div>
            <div class="stat-card" onclick="switchTab('orders')" style="cursor:pointer">
                <div class="stat-label">Fila</div><div class="stat-value">${state.orders.length}</div>
            </div>
        </div>
        <div class="card fade-in">
            <div class="card-header"><h2>Status</h2>
                <button class="btn btn-outline btn-sm" onclick="syncData()">🔄 Atualizar</button>
            </div>
            <div class="card-body" style="line-height:2;font-size:0.875rem;color:var(--text-dim)">
                ✅ <strong>cozinha_api.exe</strong> rodando como processo filho<br>
                ✅ Comunicação <strong>stdin/stdout</strong><br>
                ✅ Persistência em <code>data/*.txt</code><br>
                ✅ Operações integradas
            </div>
        </div>`;
}

// ── CATÁLOGO ──────────────────────────────────────────────────────────────────
function renderCatalog() {
    const rows = state.catalog.map(item => {
        const inStock = state.inventory.some(s => s.id_ingrediente === item.id);
        const inRecipe = state.recipes.some(r => r.ingredients.some(i => i.id === item.id));
        const blocked = inStock || inRecipe;
        let badges = [];
        if (inStock) badges.push('<span class="badge badge-green">Em estoque</span>');
        if (inRecipe) badges.push('<span class="badge badge-amber">Em receita</span>');
        return `<tr>
            <td><span class="badge badge-blue">#${item.id}</span></td>
            <td><strong>${item.name}</strong></td>
            <td>${item.unit}</td>
            <td style="text-align:right">
                ${badges.length ? `<div style="display:flex;gap:4px;justify-content:flex-end;flex-wrap:wrap">${badges.join('')}</div>` : ''}
            </td>
            <td style="text-align:right">
                ${blocked
                ? ''
                : `<button class="btn btn-danger btn-sm" onclick="delCatalog(${item.id},'${esc(item.name)}')">🗑 Remover</button>`}
            </td>
        </tr>`;
    }).join('');

    document.getElementById('main-content').innerHTML = `
        <header class="fade-in"><h1>Catálogo de Ingredientes</h1><p>Base global de insumos.</p></header>
        <div class="card fade-in">
            <div class="card-header">
                <h2>${state.catalog.length} ingrediente(s)</h2>
                <button class="btn btn-primary" onclick="modalCatalog()">+ Novo Ingrediente</button>
            </div>
            <table><thead><tr><th>ID</th><th>Nome</th><th>Unidade</th><th style="text-align:right">Situação</th><th style="text-align:right">Ação</th></tr></thead>
            <tbody>${rows || emptyRow(5, '📦', 'Nenhum ingrediente cadastrado.')}</tbody></table>
        </div>`;
}

function modalCatalog() {
    openModal(`
        <div class="modal-title">Novo Ingrediente</div>
        <div class="form-group"><label>Nome</label><input id="m-name" placeholder="Ex: Farinha de Trigo"></div>
        <div class="form-group"><label>Unidade</label><input id="m-unit" placeholder="Ex: kg, ml, un"></div>
        <div class="modal-actions">
            <button class="btn btn-outline" onclick="closeModal()">Cancelar</button>
            <button class="btn btn-primary" onclick="addCatalog()">Cadastrar</button>
        </div>`);
}

async function addCatalog() {
    const name = val('m-name'), unit = val('m-unit');
    if (!name || !unit) return toast('Preencha todos os campos.', 'error');
    const r = await api('/api/catalog', 'POST', { name, unit });
    if (r.ok) { closeModal(); await syncData(); toast(`✅ "${name}" cadastrado!`); }
    else toast('❌ ' + (r.error || 'Erro'), 'error');
}

async function delCatalog(id, name) {
    if (!confirm(`Remover "${name}" do catálogo?`)) return;
    const r = await api(`/api/catalog/${id}`, 'DELETE');
    if (r.ok) { await syncData(); toast(`✅ "${name}" removido.`); }
    else toast('❌ ' + (r.error || 'Erro ao remover'), 'error');
}

// ── ESTOQUE ───────────────────────────────────────────────────────────────────
function renderInventory() {
    const rows = state.inventory.map(item => {
        const name = catName(item.id_ingrediente), unit = catUnit(item.id_ingrediente);
        return `<tr>
            <td><span class="badge badge-blue">#${item.id_ingrediente}</span></td>
            <td><strong>${name}</strong></td>
            <td>${item.quantity.toFixed(2)} ${unit}</td>
            <td style="text-align:right;white-space:nowrap">
                <button class="btn btn-outline btn-sm" onclick="modalAddStock(${item.id_ingrediente})">+ Entrada</button>
                <button class="btn btn-danger btn-sm" onclick="delStock(${item.id_ingrediente},'${esc(name)}')">🗑</button>
            </td></tr>`;
    }).join('');

    document.getElementById('main-content').innerHTML = `
        <header class="fade-in"><h1>Estoque</h1><p>Quantidades atuais.</p></header>
        <div class="card fade-in">
            <div class="card-header">
                <h2>${state.inventory.length} tipo(s)</h2>
                <button class="btn btn-primary" onclick="modalStock()">+ Nova Entrada</button>
            </div>
            <table><thead><tr><th>ID</th><th>Ingrediente</th><th>Quantidade</th><th style="text-align:right">Ações</th></tr></thead>
            <tbody>${rows || emptyRow(4, '🏠', 'Estoque vazio.')}</tbody></table>
        </div>`;
}

function modalStock() {
    if (!state.catalog.length) return toast('Cadastre ingredientes primeiro.', 'error');
    const opts = state.catalog.map(i => `<option value="${i.id}">${i.name} (${i.unit})</option>`).join('');
    openModal(`
        <div class="modal-title">Nova Entrada</div>
        <div class="form-group"><label>Ingrediente</label><select id="m-id">${opts}</select></div>
        <div class="form-group"><label>Quantidade</label><input id="m-qtd" type="number" step="0.01" min="0.01" placeholder="Ex: 5.0"></div>
        <div class="modal-actions">
            <button class="btn btn-outline" onclick="closeModal()">Cancelar</button>
            <button class="btn btn-primary" onclick="addStock()">Confirmar</button>
        </div>`);
}

function modalAddStock(id) {
    const it = catItem(id);
    openModal(`
        <div class="modal-title">Entrada: ${it ? it.name : '#' + id}</div>
        <div class="form-group"><label>Quantidade (${it ? it.unit : ''})</label>
            <input id="m-qtd" type="number" step="0.01" min="0.01" placeholder="Ex: 2.5"></div>
        <div class="modal-actions">
            <button class="btn btn-outline" onclick="closeModal()">Cancelar</button>
            <button class="btn btn-primary" onclick="addStockById(${id})">Confirmar</button>
        </div>`);
}

async function addStock() {
    const id = parseInt(document.getElementById('m-id').value), qtd = parseFloat(val('m-qtd'));
    if (!qtd || qtd <= 0) return toast('Quantidade inválida.', 'error');
    const r = await api('/api/stock', 'POST', { id, qtd });
    if (r.ok) { closeModal(); await syncData(); toast(`✅ +${qtd} adicionado ao estoque.`); }
    else toast('❌ ' + (r.error || 'Erro'), 'error');
}

async function addStockById(id) {
    const qtd = parseFloat(val('m-qtd'));
    if (!qtd || qtd <= 0) return toast('Quantidade inválida.', 'error');
    const r = await api('/api/stock', 'POST', { id, qtd });
    if (r.ok) { closeModal(); await syncData(); toast(`✅ +${qtd} adicionado.`); }
    else toast('❌ ' + (r.error || 'Erro'), 'error');
}

async function delStock(id, name) {
    if (!confirm(`Remover "${name}" totalmente do estoque?`)) return;
    const r = await api(`/api/stock/${id}`, 'DELETE');
    if (r.ok) { await syncData(); toast(`✅ "${name}" removido do estoque.`); }
    else toast('❌ ' + (r.error || 'Erro'), 'error');
}

// ── RECEITAS ──────────────────────────────────────────────────────────────────
function renderRecipes() {
    const cards = state.recipes.map(r => {
        const ings = r.ingredients.map(ing => `
            <div class="ing-row">
                <span>${catName(ing.id)}</span>
                <span style="color:var(--text-dim)">${ing.qtd.toFixed(2)} ${catUnit(ing.id)}</span>
            </div>`).join('');
        const hasPedidos = state.orders.some(o => o.id_receita === r.id);
        return `
        <div class="card fade-in">
            <div class="card-header">
                <div><h2>${r.name}</h2><span class="badge badge-blue" style="margin-top:4px">ID #${r.id}</span></div>
                <div style="display:flex;gap:8px;flex-wrap:wrap;align-items:center">
                    <button class="btn btn-outline btn-sm" onclick="modalIngredient(${r.id})">+ Ingrediente</button>
                    <button class="btn btn-success btn-sm" onclick="addOrder(${r.id})">🛒 Pedir</button>
                    ${hasPedidos
                ? `<span class="badge badge-amber" title="Tem pedidos na fila">Na fila</span>`
                : `<button class="btn btn-danger btn-sm" onclick="delRecipe(${r.id},'${esc(r.name)}')">🗑</button>`}
                </div>
            </div>
            <div class="card-body">
                <p style="font-size:0.85rem;color:var(--text-dim);margin-bottom:1rem">${r.preparo || 'Sem modo de preparo.'}</p>
                <div class="ing-list">${ings || '<p style="font-size:0.8rem;color:var(--text-dim)">Sem ingredientes.</p>'}</div>
            </div>
        </div>`;
    }).join('');

    document.getElementById('main-content').innerHTML = `
        <header class="fade-in"><h1>Receitas</h1><p>Gerencie receitas da cozinha.</p></header>
        <div style="display:flex;justify-content:flex-end;margin-bottom:1rem" class="fade-in">
            <button class="btn btn-primary" onclick="modalRecipe()">+ Nova Receita</button>
        </div>
        ${cards || `<div class="card"><div class="card-body">${emptyRow(1, '📖', 'Nenhuma receita.')}</div></div>`}`;
}

function modalRecipe() {
    openModal(`
        <div class="modal-title">Nova Receita</div>
        <div class="form-group"><label>Nome</label><input id="m-name" placeholder="Ex: Bolo de Chocolate"></div>
        <div class="form-group"><label>Modo de Preparo</label><textarea id="m-prep" placeholder="Descreva os passos..."></textarea></div>
        <div class="modal-actions">
            <button class="btn btn-outline" onclick="closeModal()">Cancelar</button>
            <button class="btn btn-primary" onclick="addRecipe()">Criar</button>
        </div>`);
}

async function addRecipe() {
    const name = val('m-name'), preparo = val('m-prep') || 'Sem preparo.';
    if (!name) return toast('Informe o nome.', 'error');
    const r = await api('/api/recipe', 'POST', { name, preparo });
    if (r.ok) { closeModal(); await syncData(); toast(`✅ Receita "${name}" criada!`); }
    else toast('❌ ' + (r.error || 'Erro'), 'error');
}

async function delRecipe(id, name) {
    if (!confirm(`Remover a receita "${name}"?`)) return;
    const r = await api(`/api/recipe/${id}`, 'DELETE');
    if (r.ok) { await syncData(); toast(`✅ Receita "${name}" removida.`); }
    else toast('❌ ' + (r.error || 'Erro ao remover'), 'error');
}

function modalIngredient(id_rec) {
    if (!state.catalog.length) return toast('Cadastre ingredientes primeiro.', 'error');
    const opts = state.catalog.map(i => `<option value="${i.id}">${i.name} (${i.unit})</option>`).join('');
    openModal(`
        <div class="modal-title">Ingrediente → Receita #${id_rec}</div>
        <div class="form-group"><label>Ingrediente</label><select id="m-ing">${opts}</select></div>
        <div class="form-group"><label>Quantidade</label><input id="m-qtd" type="number" step="0.01" min="0.01" placeholder="Ex: 0.5"></div>
        <div class="modal-actions">
            <button class="btn btn-outline" onclick="closeModal()">Cancelar</button>
            <button class="btn btn-primary" onclick="addIngredient(${id_rec})">Adicionar</button>
        </div>`);
}

async function addIngredient(id_rec) {
    const id_ingrediente = parseInt(document.getElementById('m-ing').value);
    const qtd = parseFloat(val('m-qtd'));
    if (!qtd || qtd <= 0) return toast('Quantidade inválida.', 'error');
    const r = await api('/api/recipe/ingredient', 'POST', { id_receita: id_rec, id_ingrediente, qtd });
    if (r.ok) { closeModal(); await syncData(); toast(`✅ Ingrediente adicionado.`); }
    else toast('❌ ' + (r.error || 'Falha'), 'error');
}

// ── FILA DE PEDIDOS ───────────────────────────────────────────────────────────
function renderOrders() {
    const items = state.orders.map((o, i) => `
        <div class="queue-item fade-in">
            <div class="queue-item-info">
                <strong>${i === 0 ? '🔜 ' : ''}${o.nome_receita}</strong>
                <span>Pedido #${o.id_pedido} · ${i === 0 ? 'Próximo' : 'Posição ' + (i + 1)}</span>
            </div>
            <div style="display:flex;gap:8px;align-items:center">
                <span class="badge ${i === 0 ? 'badge-green' : 'badge-blue'}">${i === 0 ? 'PRÓXIMO' : 'FILA'}</span>
                <button class="btn btn-danger btn-sm" onclick="delOrder(${o.id_pedido})">✕</button>
            </div>
        </div>`).join('');

    document.getElementById('main-content').innerHTML = `
        <header class="fade-in"><h1>Fila de Pedidos</h1><p>Estrutura FIFO — primeiro a entrar, primeiro a sair.</p></header>
        <div class="card fade-in">
            <div class="card-header">
                <h2>${state.orders.length} pedido(s)</h2>
                <div style="display:flex;gap:8px">
                    <button class="btn btn-outline" onclick="modalOrder()">+ Novo Pedido</button>
                    <button class="btn btn-success" onclick="processOrder()" ${!state.orders.length ? 'disabled' : ''}>🍳 Processar Próximo</button>
                </div>
            </div>
            <div class="card-body">
                <div class="queue-list">
                    ${items || `<div class="empty-state"><div class="icon">⏳</div><p>Fila vazia.</p></div>`}
                </div>
            </div>
        </div>`;
}

function modalOrder() {
    if (!state.recipes.length) return toast('Cadastre receitas primeiro.', 'error');
    const opts = state.recipes.map(r => `<option value="${r.id}">${r.name}</option>`).join('');
    openModal(`
        <div class="modal-title">Novo Pedido</div>
        <div class="form-group"><label>Receita</label><select id="m-rec">${opts}</select></div>
        <div class="modal-actions">
            <button class="btn btn-outline" onclick="closeModal()">Cancelar</button>
            <button class="btn btn-primary" onclick="submitNewOrder()">Enfileirar</button>
        </div>`);
}

async function submitNewOrder() {
    const id = parseInt(document.getElementById('m-rec').value);
    await addOrder(id);
}

async function addOrder(id) {
    const r = await api('/api/order', 'POST', { id });
    if (r.ok) {
        closeModal(); await syncData();
        toast('✅ Pedido adicionado à fila! Estoque será verificado ao processar.');
        switchTab('orders');
    } else {
        toast('❌ ' + (r.error || 'Erro'), 'error', 6000);
    }
}

async function processOrder() {
    if (!state.orders.length) return toast('Fila vazia!', 'error');
    const nome = state.orders[0].nome_receita;
    const r = await api('/api/order/process', 'POST');
    await syncData();

    if (r.pilha_ops && r.pilha_ops.length > 0) {
        showPilhaLog(nome, r.ok, r.pilha_ops, r.rollback, r.error, r.falhou);
    } else if (r.rollback) {
        showPilhaLog(nome, false, [], true, r.error, r.falhou);
    } else if (r.ok) {
        toast(`✅ "${nome}" concluído!`);
    } else {
        toast('❌ ' + (r.error || 'Erro'), 'error');
    }
}

function showPilhaLog(nome, sucesso, ops, rollback, errorMsg, falhou) {
    const pushOps = ops.filter(o => o.op === 'PUSH');
    const popOps = ops.filter(o => o.op === 'POP_ROLLBACK');

    let html = `<div class="modal-title">${sucesso ? '✅ Pedido Processado' : '❌ Falha — Rollback'}</div>`;
    html += `<p style="margin-bottom:1rem;font-size:0.9rem;color:var(--text-dim)">Receita: <strong>${nome}</strong></p>`;

    if (pushOps.length > 0) {
        /* Fase 1: PUSH */
        html += `<div class="pilha-section">`;
        html += `<h3 style="font-size:0.85rem;color:var(--green);margin-bottom:0.5rem">📥 Fase 1 — Pilha: PUSH (retirada do estoque)</h3>`;
        html += `<div class="pilha-stack">`;
        pushOps.forEach((op, i) => {
            html += `<div class="pilha-item push">
                <span class="pilha-badge">PUSH #${i + 1}</span>
                <span>${op.nome || '#' + op.id}</span>
                <span class="pilha-qtd">-${op.qtd.toFixed(2)}</span>
            </div>`;
        });
        html += `</div></div>`;
    }

    /* Card do ingrediente que causou a falha */
    if (!sucesso && falhou) {
        const pct = falhou.necessario > 0 ? Math.min(100, (falhou.disponivel / falhou.necessario) * 100) : 0;
        const unit = catUnit(falhou.id) || '';
        html += `<div class="pilha-section" style="margin-top:1rem;border-color:var(--red)">
            <h3 style="font-size:0.85rem;color:var(--red);margin-bottom:0.5rem">🚫 Ingrediente insuficiente</h3>
            <div style="display:flex;align-items:center;gap:12px;padding:0.5rem">
                <div style="flex:1">
                    <strong style="font-size:0.95rem">${falhou.nome || '#' + falhou.id}</strong>
                    <div style="font-size:0.8rem;color:var(--text-dim);margin-top:4px">
                        Tem <strong style="color:var(--amber)">${falhou.disponivel.toFixed(2)} ${unit}</strong>
                        — Precisa <strong style="color:var(--red)">${falhou.necessario.toFixed(2)} ${unit}</strong>
                    </div>
                    <div style="margin-top:6px;height:6px;border-radius:3px;background:rgba(255,255,255,0.1);overflow:hidden">
                        <div style="height:100%;width:${pct}%;background:${pct > 0 ? 'var(--amber)' : 'var(--red)'};border-radius:3px;transition:width 0.3s"></div>
                    </div>
                    <div style="font-size:0.7rem;color:var(--text-dim);margin-top:2px">${pct.toFixed(0)}% disponível</div>
                </div>
            </div>
        </div>`;
    }

    /* Fase 2: POP_ROLLBACK (se houver) */
    if (popOps.length > 0) {
        html += `<div class="pilha-section" style="margin-top:1rem">`;
        html += `<h3 style="font-size:0.85rem;color:var(--red);margin-bottom:0.5rem">📤 Fase 2 — Pilha: POP + ROLLBACK (devolvendo ao estoque)</h3>`;
        html += `<div class="pilha-stack">`;
        popOps.forEach((op, i) => {
            html += `<div class="pilha-item pop">
                <span class="pilha-badge pop">POP #${i + 1}</span>
                <span>${op.nome || '#' + op.id}</span>
                <span class="pilha-qtd">+${op.qtd.toFixed(2)}</span>
            </div>`;
        });
        html += `</div></div>`;
        html += `<p style="margin-top:1rem;padding:0.75rem;border-radius:8px;background:rgba(239,68,68,0.15);color:var(--red);font-size:0.85rem">
            ⚠️ ${pushOps.length} item(s) já retirado(s) foram devolvidos ao estoque via rollback (pilha).
        </p>`;
    } else if (!sucesso && pushOps.length === 0) {
        html += `<p style="margin-top:0.75rem;padding:0.6rem;border-radius:8px;background:rgba(255,255,255,0.03);color:var(--text-dim);font-size:0.8rem">
            Nenhum PUSH realizado — a pilha permaneceu vazia. Sem necessidade de rollback.
        </p>`;
    } else if (sucesso) {
        html += `<p style="margin-top:1rem;padding:0.75rem;border-radius:8px;background:rgba(34,197,94,0.15);color:var(--green);font-size:0.85rem">
            ✅ Todos os ${pushOps.length} ingrediente(s) retirados com sucesso. Pedido concluído!
        </p>`;
    }

    html += `<div class="modal-actions"><button class="btn btn-primary" onclick="closeModal()">Fechar</button></div>`;
    openModal(html);
}

async function delOrder(id) {
    if (!confirm(`Cancelar pedido #${id}?`)) return;
    const r = await api(`/api/order/${id}`, 'DELETE');
    if (r.ok) { await syncData(); toast(`✅ Pedido #${id} cancelado.`); }
    else toast('❌ ' + (r.error || 'Erro'), 'error');
}

// ── Modal ─────────────────────────────────────────────────────────────────────
function openModal(html) {
    document.getElementById('modal-body').innerHTML = html;
    document.getElementById('modal').classList.add('open');
}
function closeModal() { document.getElementById('modal').classList.remove('open'); }
function closeModalOutside(e) { if (e.target.id === 'modal') closeModal(); }

// ── Escape para strings em atributos HTML ─────────────────────────────────────
function esc(s) { return (s || '').replace(/'/g, "\\'").replace(/"/g, '&quot;'); }
