(function(){
  var container = document.getElementById('tag-graph');
  if (!container) return;
  var data = window.__graphData || {nodes:[],edges:[]};
  if (!data.nodes.length) { container.style.display='none'; return; }

  var maxCount = Math.max.apply(null, data.nodes.map(function(n){return n.count;}));

  var nodes = new vis.DataSet(data.nodes.map(function(n) {
    return {
      id: n.id,
      label: '  ' + n.id + '  ',
      value: n.count,
      size: 15 + (n.count/maxCount)*20,
      color: { background:'#0969da', border:'#0550ae', highlight:{background:'#d73a49',border:'#a32032'} }
    };
  }));

  var edges = new vis.DataSet(data.edges.map(function(e) {
    return { from: e.source, to: e.target, color:{color:'#d0d7de'} };
  }));

  var network = new vis.Network(container, { nodes: nodes, edges: edges }, {
    physics: { solver:'forceAtlas2Based', forceAtlas2Based:{gravitationalConstant:-80, centralGravity:0.01, springLength:180, springConstant:0.05} },
    interaction: { hover:true, dragNodes:true, zoomView:false, dragView:true },
    nodes: { shape:'dot', borderWidth:2, font:{size:14,color:'#24292f',face:'system-ui'} },
    edges: { width:1.5, smooth:{type:'continuous'} }
  });

  var selected = null;
  var selected = null;
  network.on('click', function(ev) {
    if (ev.nodes.length > 0) {
      var tag = ev.nodes[0];
      if (selected === tag) { network.selectNodes([]); selected = null; filter(null); }
      else { selected = tag; filter(tag); }
    } else {
      selected = null; filter(null);
    }
  });

  function filter(tag) {
    document.querySelectorAll('.index-card, .view-all, .tag-group').forEach(function(c) {
      if (!tag) { c.classList.remove('filtered'); return; }
      var ok = false;
      if (c.classList.contains('tag-group')) {
        var h = c.querySelector('h2');
        ok = h && h.textContent.trim().startsWith(tag + ' ');
      } else if (c.classList.contains('view-all')) {
        ok = c.querySelector('a') && c.querySelector('a').href.indexOf('/tags/'+tag+'.html')>=0;
      } else {
        var t = c.querySelector('.card-tags');
        ok = t && t.textContent.indexOf(tag)>=0;
      }
      if (ok) c.classList.remove('filtered');
      else c.classList.add('filtered');
    });
  }

  // 暗色切换
  function updateTheme() {
    var dark = document.documentElement.getAttribute('data-theme')==='dark';
    var nodeColor = dark ? '#58a6ff' : '#0969da';
    var nodeBorder = dark ? '#388bfd' : '#0550ae';
    var edgeColor = dark ? '#30363d' : '#d0d7de';
    var fontColor = dark ? '#c9d1d9' : '#24292f';
    nodes.forEach(function(n) {
      nodes.update({id:n.id, color:{background:nodeColor,border:nodeBorder,highlight:{background:'#d73a49',border:'#a32032'}}});
    });
    edges.forEach(function(e) {
      edges.update({id:e.id, color:{color:edgeColor}});
    });
    network.setOptions({ nodes: { font: { color: fontColor } } });
  }

  // 缩放按钮
  document.getElementById('gfit').onclick = function() { network.fit({animation:true}); };
  document.getElementById('gin').onclick  = function() { network.moveTo({scale: network.getScale()*1.3, animation:true}); };
  document.getElementById('gout').onclick = function() { network.moveTo({scale: network.getScale()/1.3, animation:true}); };

  var origToggle = window.toggleTheme;
  window.toggleTheme = function() {
    origToggle();
    setTimeout(updateTheme, 50);
  };
})();
