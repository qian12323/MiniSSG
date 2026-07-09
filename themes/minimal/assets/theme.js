(function(){
  var el = document.documentElement;

  function updateBtn(s) {
    var b = document.querySelector('.btn-theme');
    if (b) b.textContent = s === 'dark' ? '☀️' : '🌙';
  }

  function toggleHljs(s) {
    var light = document.getElementById('hljs-light');
    var dark  = document.getElementById('hljs-dark');
    if (!light || !dark) return;
    light.disabled = s === 'dark';
    dark.disabled  = s !== 'dark';
  }

  var theme = el.getAttribute('data-theme');
  updateBtn(theme);
  toggleHljs(theme);

  window.toggleTheme = function() {
    var n = el.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
    el.setAttribute('data-theme', n);
    localStorage.setItem('theme', n);
    updateBtn(n);
    toggleHljs(n);
  };
})();
