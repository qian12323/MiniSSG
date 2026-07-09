// 引言包裹
(function(){
  var content = document.querySelector('.content');
  if (!content) return;
  var firstH = content.querySelector('h1,h2,h3,h4,h5,h6');
  var nodes = [];
  var cur = content.firstElementChild;
  while (cur && cur !== firstH) {
    nodes.push(cur);
    cur = cur.nextElementSibling;
  }
  if (!nodes.length) return;
  var wrap = document.createElement('div');
  wrap.className = 'intro-wrap';
  nodes.forEach(function(n) { wrap.appendChild(n); });
  content.insertBefore(wrap, content.firstChild);
  var img = wrap.querySelector('img');
  if (img) img.classList.add('intro-cover');
})();

// Viewer.js 图片预览
(function(){
  var s = document.createElement('script');
  s.src = 'https://cdn.jsdelivr.net/npm/viewerjs@1.11.6/dist/viewer.min.js';
  s.onload = function() {
    new Viewer(document.querySelector('.content'), {
      toolbar: { zoomIn:1, zoomOut:1, oneToOne:1, reset:1, prev:0, next:0, play:0, rotateLeft:1, rotateRight:1, flipHorizontal:1, flipVertical:1 },
    });
  };
  document.head.appendChild(s);
})();
