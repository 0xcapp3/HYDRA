(() => {

  'use strict'

  feather.replace({ 'aria-hidden': 'true' })

  var btn = document.getElementById('saveProfile');

  var phMin = document.getElementById('profilepHMin');
  var phMax = document.getElementById('profilepHMax');

  var status = document.getElementById('status');

  btn.addEventListener('click', function handleClick() {
    phMin.textContent = '3.0';
    phMax.textContent = '7.0';

    status.textContent = 'pH outside range';
    status.style = 'color: orange;'
  });

})()
