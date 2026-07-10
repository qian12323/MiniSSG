(function(){
  var data = window.__catTree || [];
  if (!data.length) return;
  var container = document.getElementById('cat-tree');

  function render(nodes, depth) {
    var ul = document.createElement('ul');
    ul.className = 'tree-list';
    nodes.forEach(function(n) {
      var li = document.createElement('li');
      li.className = 'tree-item';

      var head = document.createElement('span');
      head.className = 'tree-head';
      head.style.paddingLeft = depth * 16 + 'px';

      var toggle = document.createElement('span');
      toggle.className = 'tree-toggle';
      toggle.textContent = n.children.length ? '▶' : '';
      toggle.onclick = function(e) {
        e.stopPropagation();
        var sub = li.querySelector('.tree-sub');
        if (sub) {
          var open = sub.style.display !== 'none';
          sub.style.display = open ? 'none' : '';
          toggle.textContent = open ? '▶' : '▼';
        }
      };

      var label = document.createElement('a');
      label.className = 'tree-label';
      label.href = '#';
      label.textContent = n.name + ' (' + n.count + ')';
      label.onclick = function(e) {
        e.preventDefault();
        var active = container.querySelector('.tree-active');
        if (active === label) {
          label.classList.remove('tree-active');
          filter(null);
        } else {
          if (active) active.classList.remove('tree-active');
          label.classList.add('tree-active');
          filter(n.path, false);
        }
      };

      head.appendChild(toggle);
      head.appendChild(label);
      li.appendChild(head);

      if (n.children.length) {
        var sub = document.createElement('div');
        sub.className = 'tree-sub';
        sub.style.display = 'none';
        sub.appendChild(render(n.children, depth + 1));
        li.appendChild(sub);
      }

      ul.appendChild(li);
    });
    return ul;
  }

  container.appendChild(render(data, 0));

  // 点击 "Categories tree" 标题恢复过滤
  var heading = container.parentElement.querySelector('h3');
  if (heading) {
    heading.style.cursor = 'pointer';
    heading.addEventListener('click', function() {
      var active = container.querySelector('.tree-active');
      if (active) active.classList.remove('tree-active');
      if (window.__graphCat) filter(window.__graphCat, true);
      else document.querySelectorAll('.cat-group.filtered, .view-all.filtered').forEach(function(x){x.classList.remove('filtered');});
    });
  }

  function filter(cat, recursive) {
    if (!cat && window.__graphCat) { cat = window.__graphCat; recursive = true; }
    document.querySelectorAll('.cat-group, .view-all').forEach(function(g) {
      if (!cat) { g.classList.remove('filtered'); return; }
      var a = g.querySelector('h2 a');
      if (!a) { g.classList.add('filtered'); return; }
      var cp = a.getAttribute('href').replace('/categories/','').replace('.html','');
      var ok = recursive ? ((cp === cat) || (cp.indexOf(cat + '/') === 0)) : (cp === cat);
      if (ok) g.classList.remove('filtered'); else g.classList.add('filtered');
    });
  }
})();
