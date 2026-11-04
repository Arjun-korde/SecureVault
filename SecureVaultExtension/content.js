console.log('site: ', window.location.hostname);

function findLoginForm() {
  const passwordInput = document.querySelector("input[type='password']");
  const usernameInput = document.querySelector("input[type='email'], input[type='text']");

  if (!passwordInput || !usernameInput) return;

  console.log('login form detected');

  chrome.runtime.sendMessage({
    type: 'PAGE_DETECTED',
    domain: window.location.hostname,
  });

  attachSubmitListener(usernameInput, passwordInput);
}

function attachSubmitListener(usernameInput, passwordInput) {
  const form = passwordInput.closest('form');
  if (!form) return;

  form.addEventListener('submit', () => {
    console.log('form submitted');

    chrome.runtime.sendMessage({
      type: 'FORM_SUBMIT',
      domain: window.location.hostname,
      username: usernameInput.value,
      password: passwordInput.value,
    });
  });
}

findLoginForm();

chrome.runtime.onMessage.addListener((message) => {
  if (message.type !== 'AUTOFILL') return;

  console.log('autofilling credentials');

  const usernameInput = document.querySelector("input[type='email'], input[type='text']");
  const passwordInput = document.querySelector("input[type='password']");

  if (usernameInput && passwordInput) {
    usernameInput.value = message.username;
    passwordInput.value = message.password;
  }
});
