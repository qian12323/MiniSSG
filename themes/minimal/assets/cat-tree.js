(function(){
  var data = window.__catTree || [];
  if (!data.length) return;
  var container = document.getElementById('cat-graph');
  if (!container) return;

  var svgNS = 'http://www.w3.org/2000/svg';
  var nodes = [], active = null, svg = null, vb = {};
  var isDark = document.documentElement.getAttribute('data-theme') === 'dark';

  function countLeaves(n) {
    if (!n.children || !n.children.length) return 1;
    return n.children.reduce(function(s, c) { return s + countLeaves(c); }, 0);
  }

  var lvlH = 55, baseR = 10, totalW = countLeaves({children:data}) * 150;
  function layout(tree, depth, xBase, width) {
    if (!tree || !tree.length) return;
    var leaves = tree.reduce(function(s, n) { return s + (n.children && n.children.length ? countLeaves(n) : 1); }, 0) || 1;
    var step = width / leaves, curX = xBase;
    tree.forEach(function(n) {
      var leaf = n.children && n.children.length ? countLeaves(n) : 1;
      var maxC = Math.max.apply(null, nodes.map(function(x){return x.count||0;}).concat(1));
      var r = baseR + Math.round((n.count / Math.max(1, maxC)) * 5);
      nodes.push({ id:n.path, name:n.name, count:n.count, x:curX+leaf*step/2, y:depth*lvlH+45, r:r, leaf:n.leaf });
      if (n.children && n.children.length) layout(n.children, depth+1, curX, leaf*step);
      curX += leaf * step;
    });
  }
  layout(data, 0, 0, totalW);

  var maxY = nodes.reduce(function(m, n) { return Math.max(m, n.y); }, 0) + 50;
  var maxX = nodes.reduce(function(m, n) { return Math.max(m, n.x); }, 0) + 80;
  vb = { x:0, y:0, w:Math.max(600, maxX), h:Math.max(180, maxY) };

  svg = document.createElementNS(svgNS, 'svg');
  svg.style.width = '100%'; svg.style.height = '240px';
  svg.setAttribute('viewBox', vb.x+' '+vb.y+' '+vb.w+' '+vb.h);
  container.insertBefore(svg, container.firstChild);

  function addEdge(pn, cn) {
    var line = document.createElementNS(svgNS, 'line');
    line.setAttribute('x1', pn.x); line.setAttribute('y1', pn.y);
    line.setAttribute('x2', cn.x); line.setAttribute('y2', cn.y);
    line.setAttribute('stroke', isDark ? '#484f58' : '#afb8c1');
    line.setAttribute('stroke-width', '1.5');
    svg.insertBefore(line, svg.firstChild);
  }

  var gNodes = document.createElementNS(svgNS, 'g');
  nodes.forEach(function(n) {
    var g = document.createElementNS(svgNS, 'g');
    g.setAttribute('data-path', n.id); g.style.cursor = 'pointer';

    var circle = document.createElementNS(svgNS, 'circle');
    circle.setAttribute('cx', n.x); circle.setAttribute('cy', n.y);
    circle.setAttribute('r', n.r);
    circle.setAttribute('fill', n.leaf ? (isDark?'#58a6ff':'#0969da') : (isDark?'#30363d':'#e1e4e8'));
    circle.setAttribute('stroke', isDark?'#388bfd':'#0550ae');
    circle.setAttribute('stroke-width', '2');
    g.appendChild(circle);

    var text = document.createElementNS(svgNS, 'text');
    text.setAttribute('x', n.x + n.r + 6);
    text.setAttribute('y', n.y + 4);
    text.setAttribute('fill', isDark?'#c9d1d9':'#24292f');
    text.setAttribute('font-size', '11'); text.setAttribute('font-family', 'system-ui');
    text.textContent = n.name;
    g.appendChild(text);

    g.addEventListener('click', function() {
      if (active === n.id) { active=null; window.__graphCat=null; doFilter(null); doHighlight(null); syncSidebar(null); }
      else { active=n.id; window.__graphCat=n.id; doFilter(n.id); doHighlight(n.id); syncSidebar(n.id); }
    });
    gNodes.appendChild(g);
    n._el = g;
  });
  svg.appendChild(gNodes);

  function drawEdges(tree) {
    tree.forEach(function(p) {
      var pn = nodes.find(function(nd){return nd.id===p.path;});
      if (p.children) p.children.forEach(function(c) {
        var cn = nodes.find(function(nd){return nd.id===c.path;});
        if (pn && cn) addEdge(pn, cn);
        drawEdges([c]);
      });
    });
  }
  drawEdges(data);

  function doHighlight(id) {
    nodes.forEach(function(n) {
      var c = n._el.querySelector('circle');
      if (n.id === id) { c.setAttribute('fill','#d73a49'); c.setAttribute('stroke','#a32032'); }
      else { c.setAttribute('fill', n.leaf?(isDark?'#58a6ff':'#0969da'):(isDark?'#30363d':'#e1e4e8')); c.setAttribute('stroke', isDark?'#388bfd':'#0550ae'); }
    });
  }

  function doFilter(cat) {
    document.querySelectorAll('.cat-group, .view-all').forEach(function(g) {
      if (!cat) { g.classList.remove('filtered'); return; }
      var a = g.querySelector('h2 a');
      if (!a) { g.classList.add('filtered'); return; }
      var cp = a.getAttribute('href').replace('/categories/','').replace('.html','');
      var ok = (cp === cat) || (cp.indexOf(cat + '/') === 0);
      if (ok) g.classList.remove('filtered'); else g.classList.add('filtered');
    });
  }

  function syncSidebar(cat) {
    var tree = document.getElementById('cat-tree');
    if (!tree) return;
    // 收起全部
    tree.querySelectorAll('.tree-sub').forEach(function(s) { s.style.display = 'none'; });
    tree.querySelectorAll('.tree-toggle').forEach(function(t) { if (t.textContent==='▼') t.textContent='▶'; });
    tree.querySelectorAll('.tree-label').forEach(function(l) { l.classList.remove('tree-active'); });
    tree.querySelectorAll('.tree-item').forEach(function(i) { i.style.display = ''; });

    if (!cat) return;

    // 找到选中节点，展开它到根的路径，隐藏不相关的
    tree.querySelectorAll('.tree-label').forEach(function(l) {
      var li = l.closest('.tree-item');
      var path = li ? getPath(li) : '';
      var ok = (path === cat) || (path.indexOf(cat + '/') === 0) || (cat.indexOf(path + '/') === 0);
      if (!ok && path !== '') { li.style.display = 'none'; }
    });

    // 对可见节点，展开其子节点
    tree.querySelectorAll('.tree-item').forEach(function(li) {
      if (li.style.display === 'none') return;
      var sub = li.querySelector('.tree-sub');
      if (sub) sub.style.display = '';
      var toggle = li.querySelector('.tree-head .tree-toggle');
      if (toggle && sub) toggle.textContent = '▼';
    });
  }

  function getPath(li) {
    var parts = [];
    var cur = li;
    while (cur) {
      var label = cur.querySelector('.tree-head .tree-label');
      if (label) parts.unshift(label.textContent.split(' ')[0]);
      var parent = cur.parentElement;
      cur = parent ? parent.closest('.tree-item') : null;
    }
    return parts.join('/');
  }

  // 缩放
  function updateView(dw, dh) {
    var v = svg.viewBox.baseVal;
    var cx = v.x + v.width/2, cy = v.y + v.height/2;
    var w = v.width * dw, h = v.height * dh;
    svg.setAttribute('viewBox', (cx-w/2)+' '+(cy-h/2)+' '+w+' '+h);
  }
  function fit() { svg.setAttribute('viewBox', vb.x+' '+vb.y+' '+vb.w+' '+vb.h); }
  function zoomIn() { updateView(0.8, 0.8); }
  function zoomOut() { updateView(1.25, 1.25); }

  var bFit = document.getElementById('cg-fit');
  var bIn  = document.getElementById('cg-in');
  var bOut = document.getElementById('cg-out');
  if (bFit) bFit.onclick = fit;
  if (bIn)  bIn.onclick  = zoomIn;
  if (bOut) bOut.onclick = zoomOut;

  // 点标题恢复
  var h = container.parentElement.querySelector('h1');
  if (h) { h.style.cursor='pointer'; h.addEventListener('click',function(){active=null;window.__graphCat=null;doFilter(null);doHighlight(null);syncSidebar(null);}); }

  // 暗色切换
  function updateColors() {
    isDark = document.documentElement.getAttribute('data-theme')==='dark';
    nodes.forEach(function(n) {
      var c = n._el.querySelector('circle');
      if (active === n.id) return;
      c.setAttribute('fill', n.leaf?(isDark?'#58a6ff':'#0969da'):(isDark?'#30363d':'#e1e4e8'));
      c.setAttribute('stroke', isDark?'#388bfd':'#0550ae');
    });
    svg.querySelectorAll('text').forEach(function(t){t.setAttribute('fill',isDark?'#c9d1d9':'#24292f');});
    svg.querySelectorAll('line').forEach(function(l){l.setAttribute('stroke',isDark?'#484f58':'#afb8c1');});
  }
  var _orig = window.toggleTheme;
  window.toggleTheme = function() { _orig(); setTimeout(updateColors,50); };
})();
