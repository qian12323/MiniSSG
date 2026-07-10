(function(){
  var data = window.__catTree || [];
  if (!data.length) return;
  var container = document.getElementById('cat-graph');
  if (!container) return;

  var isDark = document.documentElement.getAttribute('data-theme')==='dark';
  var active = null;
  var cLeaf = isDark ? '#a78bfa' : '#7c3aed';
  var cBranch = isDark ? '#374151' : '#e5e7eb';
  var cBorder = isDark ? '#4b5563' : '#d1d5db';
  var cText = isDark ? '#c9d1d9' : '#374151';

  function countLeaves(n) {
    if (!n.children || !n.children.length) return 1;
    return n.children.reduce(function(s, c) { return s + countLeaves(c); }, 0);
  }

  // 手动布局：自适应间距
  var totalLeaves = countLeaves({children:data});
  var lvlH = 90, unitW = Math.max(130, 800 / Math.max(1, totalLeaves));
  var nodesArr = [], edgesArr = [];

  function layout(tree, depth, xBase, width) {
    if (!tree || !tree.length) return;
    var leaves = tree.reduce(function(s, n) { return s + (n.children && n.children.length ? countLeaves(n) : 1); }, 0) || 1;
    var step = width / leaves, curX = xBase;
    tree.forEach(function(n) {
      var leaf = n.children && n.children.length ? countLeaves(n) : 1;
      var cx = curX + leaf * step / 2;
      var cy = depth * lvlH + 50;
      nodesArr.push({
        id: n.path, label: n.name, value: Math.max(n.count||0,1),
        x: cx, y: cy, fixed: { x:true, y:true },
        color: { background: n.leaf ? cLeaf : cBranch, border: cBorder,
                 highlight: { background:'#ef4444', border:'#dc2626' } }
      });
      if (n.children && n.children.length) layout(n.children, depth+1, curX, leaf*step);
      curX += leaf * step;
    });
  }
  layout(data, 0, 0, totalLeaves * unitW);

  // 边
  function addEdges(tree, parent) {
    tree.forEach(function(n) {
      if (parent) edgesArr.push({ from: parent, to: n.path, color: { color: isDark?'#374151':'#d1d5db' } });
      if (n.children && n.children.length) addEdges(n.children, n.path);
    });
  }
  addEdges(data, null);

  var network = new vis.Network(container, {
    nodes: new vis.DataSet(nodesArr),
    edges: new vis.DataSet(edgesArr)
  }, {
    physics: false,
    interaction: { hover: true, zoomView: false, dragView: true, selectConnectedEdges: false },
    nodes: { shape: 'dot', borderWidth: 2, font: { size: 12, color: cText, face:'system-ui' }, scaling: { min: 8, max: 14, label: { enabled: false } } },
    edges: { width: 1.5, smooth: { type: 'cubicBezier', forceDirection: 'vertical', roundness: 0.3 } }
  });

  network.fit({ animation: false });

  network.on('click', function(ev) {
    if (ev.nodes.length) {
      var id = ev.nodes[0];
      if (active === id) { active = null; window.__graphCat = null; doFilter(null); syncSidebar(null); network.selectNodes([]); }
      else { active = id; window.__graphCat = id; doFilter(id); syncSidebar(id); network.selectNodes([id]); }
    } else { active = null; window.__graphCat = null; doFilter(null); syncSidebar(null); network.selectNodes([]); }
  });

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
    var tree = document.getElementById('cat-tree'); if (!tree) return;
    tree.querySelectorAll('.tree-sub').forEach(function(s){s.style.display='none';});
    tree.querySelectorAll('.tree-toggle').forEach(function(t){if(t.textContent==='▼')t.textContent='▶';});
    tree.querySelectorAll('.tree-label').forEach(function(l){l.classList.remove('tree-active');});
    tree.querySelectorAll('.tree-item').forEach(function(i){i.style.display='';});
    if (!cat) return;
    tree.querySelectorAll('.tree-label').forEach(function(l) {
      var li=l.closest('.tree-item'), path=li?getPath(li):'';
      if (path&&path!==cat&&path.indexOf(cat+'/')!==0&&cat.indexOf(path+'/')!==0) li.style.display='none';
    });
    tree.querySelectorAll('.tree-item').forEach(function(li){
      if(li.style.display==='none')return; var s=li.querySelector('.tree-sub');
      if(s)s.style.display=''; var t=li.querySelector('.tree-head .tree-toggle'); if(t&&s)t.textContent='▼';
    });
  }

  function getPath(li) {
    var p=[]; while(li){var l=li.querySelector('.tree-head .tree-label'); if(l)p.unshift(l.textContent.split(' ')[0]); li=li.parentElement?li.parentElement.closest('.tree-item'):null;} return p.join('/');
  }

  document.getElementById('cg-fit').onclick = function(){network.fit({animation:true});};
  document.getElementById('cg-in').onclick  = function(){network.moveTo({scale:network.getScale()*1.3,animation:true});};
  document.getElementById('cg-out').onclick = function(){network.moveTo({scale:network.getScale()/1.3,animation:true});};

  var h = container.parentElement.parentElement.querySelector('h1');
  if (h) { h.style.cursor='pointer'; h.addEventListener('click',function(){active=null;window.__graphCat=null;doFilter(null);syncSidebar(null);network.selectNodes([]);}); }

  function updateTheme() {
    isDark=document.documentElement.getAttribute('data-theme')==='dark';
    cLeaf=isDark?'#a78bfa':'#7c3aed';
    cBranch=isDark?'#374151':'#e5e7eb';
    cBorder=isDark?'#4b5563':'#d1d5db';
    cText=isDark?'#c9d1d9':'#374151';
    network.setOptions({nodes:{font:{color:cText}},edges:{color:{color:isDark?'#374151':'#d1d5db'}}});
    var ns=network.body.data.nodes;
    ns.forEach(function(n){ns.update({id:n.id,color:{background:n.label?(cLeaf):cBranch,border:cBorder,highlight:{background:'#ef4444',border:'#dc2626'}}});});
  }
  var _orig=window.toggleTheme;
  window.toggleTheme=function(){_orig();setTimeout(updateTheme,50);};
})();
