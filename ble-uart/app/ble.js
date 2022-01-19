const deviceServiceUuid = "9d019f5b-f367-4bcb-83aa-e8cae9854312";
const deviceServiceCharacteristicWriteUuid =
  "c143b70a-3f7f-49f8-84c1-90b6c01eb6bb";
const deviceServiceCharacteristicNotifyUuid =
  "c0e78027-e4aa-4a17-9e3e-3f252a2e0856";

let deviceServiceCharacteristicWrite;
let deviceServiceCharacteristicNotify;

let device;
let statusConnected = false;

let receiveText = "";

// UIコンポーネント取得
function getViewDeviceConnectionStatus() {
  return document.getElementById("connection-status");
}
function getViewDeviceReceiveMessage() {
  return document.getElementById("textarea-receive-message");
}

// Device系
async function connect() {
  if (statusConnected) {
    console.log("Disconnected");
    statusConnected = false;
    displayUpdate();
  }
  device = await navigator.bluetooth.requestDevice({
    acceptAllDevices: true,
    optionalServices: [deviceServiceUuid],
  });
  const server = await device.gatt.connect();
  let service = await server.getPrimaryService(deviceServiceUuid);
  deviceServiceCharacteristicWrite = await service.getCharacteristic(
    deviceServiceCharacteristicWriteUuid
  );

  deviceServiceCharacteristicNotify = await service.getCharacteristic(
    deviceServiceCharacteristicNotifyUuid
  );
  await deviceServiceCharacteristicNotify.startNotifications();
  deviceServiceCharacteristicNotify.addEventListener(
    "characteristicvaluechanged",
    deviceServiceNotifyCallback
  );

  device.ongattserverdisconnected = disconnect;
  statusConnected = true;
  displayUpdate();
  console.log("Connected");
}

function deviceServiceNotifyCallback(event) {
  const decoder = new TextDecoder("utf-8");
  try {
    const value = decoder.decode(event.target.value);
    console.log(value);
    receiveText += value;
    getViewDeviceReceiveMessage().innerText = receiveText;
  } catch {
    console.log("data decode error");
  }
}

function displayUpdate() {
  if (statusConnected) {
    getViewDeviceConnectionStatus().innerText = "接続済み";
    getViewDeviceConnectionStatus().setAttribute("class", "btn btn-primary");
  } else {
    getViewDeviceConnectionStatus().innerText = "未接続";
    getViewDeviceConnectionStatus().setAttribute("class", "btn btn-secondary");
  }
}

async function disconnect() {
  await device.gatt.disconnect();
  console.log("Disconnected");
  window.alert("disconnected");
  statusConnected = false;
  displayUpdate();
}

window.onload = function () {
  displayUpdate();
};
