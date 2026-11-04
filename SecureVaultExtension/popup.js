document.getElementById('saveBtn').addEventListener('click', () => {
  chrome.runtime.sendMessage({
    type: 'USER_CONFIRMED_SAVE',
  });
  window.close();
});

document.getElementById('cancelBtn').addEventListener('click', () => {
  window.close();
});
