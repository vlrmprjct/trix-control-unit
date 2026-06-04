const $ = id => document.getElementById(id);
const encA = $('encA'), barA = $('barA'), rawA = $('rawA'), dirAEl = $('dirA');
const encB = $('encB'), barB = $('barB'), rawB = $('rawB'), dirBEl = $('dirB');
const speedKmh = $('speedKmh'), speedRaw = $('speedRaw');
const trackTable = $('trackTable');
const relayGrid = $('relayGrid');
const logEl = $('log');
const statusEl = $('status');
const connectBtn = $('connectBtn');
const firmwareEl = $('firmware');

let port = null;
let reader = null;

// SEND DATA TO ARDUINO (SimpleWebSerial format)
const sendToArduino = async (event, data) => {
    if (!port?.writable) return;
    const writer = port.writable.getWriter();
    const payload = JSON.stringify([event, data]) + '\n';
    await writer.write(new TextEncoder().encode(payload));
    writer.releaseLock();
    log('> ' + event + ' ' + JSON.stringify(data));
};

const log = msg => {
    const ts = new Date().toLocaleTimeString();
    logEl.textContent = `[${ts}] ${msg}\n` + logEl.textContent;
    if (logEl.textContent.length > 2000) logEl.textContent = logEl.textContent.slice(0, 2000);
};

const setBar = (bar, val) => {
    bar.style.width = Math.min(Math.abs(val) / 255 * 100, 100) + '%';
};

const setDir = (el, dir) => {
    el.className = 'dir ' + (dir > 0 ? 'cw' : dir < 0 ? 'ccw' : 'stop');
    el.textContent = dir > 0 ? 'CW' : dir < 0 ? 'CCW' : 'STOP';
};

const dot = state =>
    '<span class="dot ' + (state ? 'on' : 'off') + '"></span>';

// ACCUMULATED STATE FROM CHUNKED EVENTS
const state = { tracks: {}, relays: [] };

const updateCommon = payload => {
    encA.textContent = payload.encA;
    encB.textContent = payload.encB;
    setBar(barA, payload.encA);
    setBar(barB, payload.encB);
    rawA.textContent = 'raw: ' + payload.rawA;
    rawB.textContent = 'raw: ' + payload.rawB;
    setDir(dirAEl, payload.dirA);
    setDir(dirBEl, payload.dirB);
    speedKmh.textContent = payload.spdKmh > 0 ? payload.spdKmh + ' km/h' : '-- km/h';
    speedRaw.textContent = payload.spdRaw > 0 ? payload.spdRaw.toFixed(1) + ' cm/s' : '-- cm/s';
    if (payload.firmware) firmwareEl.textContent = payload.firmware;
};

const updateTracks = payload => {
    // Map short Arduino keys to readable names
    for (const [name, t] of Object.entries(payload)) {
        state.tracks[name] = {
            selected: !!t.s,
            powered:  !!t.p,
            occupied: !!t.o,
            pending:  !!t.n,
        };
    }
    let html = '';
    for (const [name, track] of Object.entries(state.tracks)) {
        html += '<tr>'
            + '<td>' + name.toUpperCase() + '</td>'
            + '<td>' + dot(track.selected) + '</td>'
            + '<td>' + dot(track.powered) + '</td>'
            + '<td>' + dot(track.occupied) + '</td>'
            + '</tr>';
    }
    trackTable.innerHTML = html;
    updateTrackColors(state.tracks);
};

const updateRelays = payload => {
    let html = '';
    for (let i = 0; i < payload.length; i++) {
        html += '<div class="relay-cell ' + (payload[i] ? 'on' : 'off') + '">'
            + (i + 1) + '</div>';
    }
    relayGrid.innerHTML = html;
};

const handleData = parsed => {
    if (!Array.isArray(parsed) || parsed.length < 2) { log(JSON.stringify(parsed)); return; }
    const [evt, payload] = parsed;
    switch (evt) {
        case 'common': updateCommon(payload); break;
        case 'hbf':    updateTracks(payload); break;
        case 'bbf':    updateTracks(payload); break;
        case 'block':  updateTracks(payload); break;
        case 'relay':  updateRelays(payload); break;
        default: log(evt + ': ' + JSON.stringify(payload));
    }
};

connectBtn.onclick = async () => {
    if (port) { log('ALREADY CONNECTED'); return; }
    if (!('serial' in navigator)) { log('WEB SERIAL NOT SUPPORTED'); return; }

    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 9600, bufferSize: 16384 });
    } catch (e) {
        log('OPEN ERROR: ' + e.message);
        port = null;
        return;
    }

    connectBtn.textContent = 'Connected';
    connectBtn.classList.add('active');
    log('SERIAL CONNECTED');

    const decoder = new TextDecoder();
    let buf = '';

    const read = async () => {
        while (port && port.readable) {
            const r = port.readable.getReader();
            try {
                while (true) {
                    const { value, done } = await r.read();
                    if (done) break;
                    buf += decoder.decode(value, { stream: true });
                    const lines = buf.split('\n');
                    buf = lines.pop();
                    for (const raw of lines) {
                        const line = raw.trim();
                        if (!line) continue;
                        console.log('[RAW]', line);
                        try {
                            const parsed = JSON.parse(line);
                            const evt = Array.isArray(parsed) ? parsed[0] : '?';
                            log('< ' + evt + ' ' + JSON.stringify(parsed[1]));
                            handleData(parsed);
                        } catch (pe) {
                            log(line);
                        }
                    }
                }
            } catch (e) {
                log('READ ERROR: ' + e.message);
            } finally {
                try { r.releaseLock(); } catch {}
            }
        }
    };
    read();
};

// SPEED SLIDERS
const sliderA = $('sliderA'), sliderValA = $('sliderValA');
const sliderB = $('sliderB'), sliderValB = $('sliderValB');

sliderA.oninput = () => {
    sliderValA.textContent = sliderA.value;
    sendToArduino('setSpeed', { zone: 0, value: parseInt(sliderA.value) });
};
sliderB.oninput = () => {
    sliderValB.textContent = sliderB.value;
    sendToArduino('setSpeed', { zone: 1, value: parseInt(sliderB.value) });
};
