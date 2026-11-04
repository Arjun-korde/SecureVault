console.log('service worker started');

let nativePort = null;
let pendingSaveRequest = null;
let alreadyExists = false;

// just for testing
const CURRENT_USER_ID = 2;

function ensureNativeConnection() {
  if (nativePort) return;

  console.log('connecting to native host');
  nativePort = chrome.runtime.connectNative('com.securevault.nativehost');
  console.log('connected to native host successfully');

  nativePort.onMessage.addListener(handleNativeMessage);

  nativePort.onDisconnect.addListener(() => {
    console.log('native host disconnected');
    console.log('error : ', chrome.runtime.lastError);
    nativePort = null;
  });
}

function handleNativeMessage(message) {
  console.log('Message From native:', message);

  if (message.type === 'CHECK_RESULT' && message.found) {
    alreadyExists = true;

    // Send credentials to content script for autofill
    chrome.tabs.query({ active: true, currentWindow: true }, (tabs) => {
      chrome.tabs.sendMessage(tabs[0].id, {
        type: 'AUTOFILL',
        username: message.username,
        password: message.password,
      });
    });
  } else {
    alreadyExists = false;
  }

  if (message.type === 'SAVE_COMPLETE') {
    console.log('Password Saved.');
  }
}

chrome.runtime.onMessage.addListener((msg) => {
  ensureNativeConnection();

  if (msg.type === 'PAGE_DETECTED') {
    nativePort.postMessage({
      type: 'CHECK_CREDENTIALS',
      userId: CURRENT_USER_ID,
      domain: msg.domain,
    });
  }

  if (msg.type === 'FORM_SUBMIT' && !alreadyExists) {
    // store temporarily
    pendingSaveRequest = {
      domain: msg.domain,
      username: msg.username,
      password: msg.password,
    };

    chrome.action.openPopup();
  }

  // from popup when user clicks Save button only then send the request to save the password
  if (msg.type === 'USER_CONFIRMED_SAVE' && pendingSaveRequest) {
    console.log('Received : SAVE_CONFIRMED');
    nativePort.postMessage({
      type: 'SAVE_CONFIRMED',
      userId: CURRENT_USER_ID,
      ...pendingSaveRequest,
    });

    pendingSaveRequest = null;
  }
});
