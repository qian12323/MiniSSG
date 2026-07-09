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
    html += '<li class="toc-' + h.tagName.toLowerCase() + '"><a href="#' + id + '" data-heading="' + id + '">' + label + '</a></li>';
  });

  html += '</ul>';
  toc.querySelector('.toc-title').insertAdjacentHTML('afterend', html);

  // 回到顶部按钮
  var back = document.createElement('a');
  back.className = 'toc-back-top';
  back.href = '#';
  back.textContent = '↑ Back to top';
  back.onclick = function(e) { e.preventDefault(); window.scrollTo({top:0,behavior:'smooth'}); };
  toc.appendChild(back);

  // 滚动高亮当前标题
  var links = toc.querySelectorAll('a[data-heading]');
  var observer = new IntersectionObserver(function(entries) {
    entries.forEach(function(e) {
      var id = e.target.id;
      var link = toc.querySelector('a[data-heading="' + id + '"]');
      if (!link) return;
      if (e.isIntersecting) {
        links.forEach(function(l) { l.classList.remove('active'); });
        link.classList.add('active');
      }
    });
  }, { rootMargin: '-20% 0px -70% 0px' });

  hs.forEach(function(h) { observer.observe(h); });
})();
