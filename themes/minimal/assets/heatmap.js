(function(){
  var dates = window.__postDates || [];
  var counts = window.__postCounts || {};

  var allYears = {};
  dates.forEach(function(d) { allYears[d.slice(0,4)] = 1; });
  var years = Object.keys(allYears).sort();
  if (!years.length) years.push(String(new Date().getFullYear()));
  var curYear = years[years.length-1];

  function fmt(d) { return d.getFullYear()+'-'+String(d.getMonth()+1).padStart(2,'0')+'-'+String(d.getDate()).padStart(2,'0'); }

  var hm = document.getElementById('heatmap');

  function render(year) {
    curYear = year;
    var start = new Date(year+'-01-01');
    var end = new Date(year+'-12-31');

    // 找到起始周的周日
    var s = new Date(start);
    s.setDate(s.getDate() - s.getDay());

    var totalDays = Math.ceil((end - s) / 86400000) + 1;
    var cols = Math.ceil(totalDays / 7);

    var cells = [];
    for (var d = new Date(s), i = 0; i < totalDays; d.setDate(d.getDate()+1), i++) {
      var key = fmt(d);
      cells.push({ date:key, count:counts[key]||0, inRange: d>=start && d<=end });
    }

    // 月份标签
    var months = [{col:0, label:'Jan'},{col:0, label:'Feb'},{col:0, label:'Mar'},{col:0, label:'Apr'},
                  {col:0, label:'May'},{col:0, label:'Jun'},{col:0, label:'Jul'},{col:0, label:'Aug'},
                  {col:0, label:'Sep'},{col:0, label:'Oct'},{col:0, label:'Nov'},{col:0, label:'Dec'}];
    for (var m = 0; m < 12; m++) {
      var firstDay = new Date(year+'-'+String(m+1).padStart(2,'0')+'-01');
      var diff = Math.floor((firstDay - s) / 86400000);
      months[m].col = Math.floor(diff / 7);
    }

    var html = '<div class="hm-header">'
             + '<span>Post Activity</span>'
             + '<select class="hm-year">';
    years.forEach(function(y) {
      html += '<option value="'+y+'"'+(y===year?' selected':'')+'>'+y+'</option>';
    });
    html += '</select></div>';

    html += '<div class="hm-grid-wrap"><div class="hm-months" style="grid-template-columns:30px repeat('+cols+',13px)">';
    months.forEach(function(m) {
      html += '<span style="grid-column:'+(m.col+2)+'">'+m.label+'</span>';
    });
    html += '</div>';

    html += '<div class="hm-grid" style="grid-template-columns:30px repeat('+cols+',13px)">';
    var days = ['Sun','Mon','Tue','Wed','Thu','Fri','Sat'];
    for (var r = 0; r < 7; r++) {
      html += '<span class="hm-day">'+(r%2?days[r]:'')+'</span>';
      for (var c = 0; c < cols; c++) {
        var idx = c * 7 + r;
        if (idx >= cells.length || !cells[idx].inRange) {
          html += '<span class="hm-cell hm-future"></span>';
          continue;
        }
        var cell = cells[idx];
        var cls = 'hm-cell';
        if (cell.count > 3) cls += ' hm-l4';
        else if (cell.count > 2) cls += ' hm-l3';
        else if (cell.count > 1) cls += ' hm-l2';
        else if (cell.count > 0) cls += ' hm-l1';
        html += '<span class="'+cls+'" data-date="'+cell.date+'" title="'+cell.date+': '+cell.count+' post(s)"></span>';
      }
    }
    html += '</div>';

    html += '<div class="hm-legend"><span>Less</span>'
          + '<span class="hm-cell hm-l0"></span><span class="hm-cell hm-l1"></span>'
          + '<span class="hm-cell hm-l2"></span><span class="hm-cell hm-l3"></span>'
          + '<span class="hm-cell hm-l4"></span><span>More</span></div></div>';

    hm.innerHTML = html;

    // 年份切换
    hm.querySelector('.hm-year').addEventListener('change', function() {
      render(this.value);
    });

    // 点击过滤
    hm.addEventListener('click', function(e) {
      var cell = e.target.closest('.hm-cell');
      if (!cell || !cell.dataset.date) return;
      var allCards = document.querySelectorAll('.index-card');
      var active = hm.querySelector('.hm-active');
      if (active === cell) {
        cell.classList.remove('hm-active');
        allCards.forEach(function(c) { c.style.display = ''; });
        return;
      }
      if (active) active.classList.remove('hm-active');
      cell.classList.add('hm-active');
      allCards.forEach(function(card) {
        var cd = card.querySelector('.card-date');
        if (!cd) return;
        card.style.display = (cd.textContent.trim() === cell.dataset.date) ? '' : 'none';
      });
    });
  }

  render(curYear);
})();
