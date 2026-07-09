(function(){
  var toc = document.getElementById('toc');
  var c = document.querySelector('.content');
  if (!c) { toc.querySelector('.toc-title').insertAdjacentHTML('afterend','<p class="no-toc">No headings</p>'); return; }

  var hs = c.querySelectorAll('h1,h2,h3');
  if (!hs.length) { toc.querySelector('.toc-title').insertAdjacentHTML('afterend','<p class="no-toc">No headings</p>'); return; }

  var cnt = [0,0,0];
  var html = '<ul>';
  var backendNum = window.__autoNumber || false;

  hs.forEach(function(h) {
    var level = parseInt(h.tagName[1]) - 1;
    for (var i = level + 1; i < 3; i++) cnt[i] = 0;
    cnt[level]++;

    var num = '';
    for (var i = 0; i <= level; i++) {
      if (cnt[i] === 0) break;
      if (num) num += '.';
      num += cnt[i];
    }

    var id = 's' + num.replace(/\./g, '-');
    h.id = id;

    var label = backendNum ? h.textContent : (num + ' ' + h.textContent);
    html += '<li class="toc-' + h.tagName.toLowerCase() + '"><a href="#' + id + '">' + label + '</a></li>';
  });

  html += '</ul>';
  toc.querySelector('.toc-title').insertAdjacentHTML('afterend', html);
})();
