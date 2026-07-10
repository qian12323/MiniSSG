(function(){
  var graphData = window.__filterTagGraph || {nodes:[],edges:[]};
  var catData = window.__filterCatCloud || [];
  var activeTags = new Set();
  var activeCats = new Set();
  var miniNetwork = null;

  function applyFilter() {
    document.querySelectorAll('.index-card').forEach(function(c) {
      var ok = true;
      if (activeTags.size) {
        var t = c.querySelector('.card-tags');
        var has = false;
        if (t) activeTags.forEach(function(tag) { if (t.textContent.indexOf(tag)>=0) has=true; });
        ok = ok && has;
      }
      if (activeCats.size) {
        var link = c.querySelector('.card-footer a');
        var has = false;
        if (link) {
          var cat = link.getAttribute('href').replace('/categories/','').replace('.html','');
          activeCats.forEach(function(sc) {
            if (cat === sc || cat.indexOf(sc+'/')===0) has = true;
          });
        }
        ok = ok && has;
      }
      if (ok) c.classList.remove('filtered');
      else c.classList.add('filtered');
    });
  }

  function resetAll() {
    activeTags.clear(); activeCats.clear();
    if (miniNetwork) miniNetwork.selectNodes([]);
    document.querySelectorAll('.fc-active').forEach(function(x){x.classList.remove('fc-active');});
    document.querySelectorAll('.index-card.filtered').forEach(function(x){x.classList.remove('filtered');});
  }

  function toggleTag(id) {
    if (activeTags.has(id)) activeTags.delete(id);
    else activeTags.add(id);
    if (miniNetwork) miniNetwork.selectNodes(Array.from(activeTags));
    applyFilter();
  }

  function toggleCat(id) {
    if (activeCats.has(id)) activeCats.delete(id);
    else activeCats.add(id);
    document.querySelectorAll('.fc-item').forEach(function(x){
      x.classList.toggle('fc-active', activeCats.has(x.dataset.id));
    });
    applyFilter();
  }

  // ---- 标签 mini 图谱 ----
  var gc = document.getElementById('filter-graph');
  if (gc && graphData.nodes.length) {
    var total = graphData.nodes.length;
    var cx = 100, cy = 90, radius = 60;

    var nodesArr = graphData.nodes.map(function(n, idx) {
      var angle = (idx/total)*Math.PI*2 - Math.PI/2;
      return {
        id: n.id, label: n.id, value: n.count,
        x: cx + Math.cos(angle)*radius,
        y: cy + Math.sin(angle)*radius,
        fixed: { x:true, y:true },
        color: { background:'#0969da', border:'#0550ae' }
      };
    });

    var vnodes = new vis.DataSet(nodesArr);
    var vedges = new vis.DataSet(graphData.edges.map(function(e) {
      return { from: e.source, to: e.target, color:{color:'#d0d7de'}, width:0.8 };
    }));

    miniNetwork = new vis.Network(gc, { nodes: vnodes, edges: vedges }, {
      physics: false,
      interaction: { hover:true, dragNodes:false, zoomView:false, dragView:false },
      nodes: { shape:'dot', borderWidth:1.5, font:{size:9,color:'#24292f',face:'system-ui'},
               scaling: { min:10, max:13, label:{enabled:false} },
               chosen: { node: function(v) { v.color='#d73a49'; v.borderColor='#a32032'; } } },
      edges: { smooth:{type:'continuous'} }
    });

    miniNetwork.on('click', function(ev) {
      if (ev.nodes.length) { toggleTag(ev.nodes[0]); }
      else { resetAll(); }
    });

    gc._mini = miniNetwork;
  }

  // ---- 分类词云 ----
  var wc = document.getElementById('filter-cats');
  if (wc && catData.length) {
    var maxCat = Math.max.apply(null, catData.map(function(c){return c.count;}));
    catData.forEach(function(c) {
      var ratio = c.count / maxCat;
      var el = document.createElement('span');
      el.className = 'fc-item';
      el.style.fontSize = (11+ratio*8)+'px';
      el.style.padding = (1+ratio*3)+'px '+(6+ratio*6)+'px';
      el.style.opacity = 0.6+ratio*0.4;
      el.textContent = c.id.indexOf('/') >= 0 ? c.id.split('/').pop() : c.id;
      el.dataset.id = c.id;
      el.onclick = function(e) { e.stopPropagation(); toggleCat(c.id); };
      wc.appendChild(el);
    });
    wc.onclick = function(e) { if (e.target === wc) resetAll(); };
  }

  document.getElementById('filterReset').onclick = resetAll;

  function updateMiniTheme() {
    if (!miniNetwork) return;
    var dark = document.documentElement.getAttribute('data-theme')==='dark';
    miniNetwork.setOptions({
      nodes: { font: { color: dark?'#c9d1d9':'#24292f' } },
      edges: { color: { color: dark?'#30363d':'#d0d7de' } }
    });
  }
  var _origToggle = window.toggleTheme;
  window.toggleTheme = function() { _origToggle(); setTimeout(updateMiniTheme, 50); };
})();
