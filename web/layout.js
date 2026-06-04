const css = getComputedStyle(document.documentElement);
const COLOR = {
    occupied: css.getPropertyValue('--color-occupied').trim(),
    selected: css.getPropertyValue('--color-selected').trim(),
    inactive: css.getPropertyValue('--color-inactive').trim(),
};

// Expand straight segments into individual cells { row, col, ids }
const expandRoute = route =>
    route.flatMap(seg =>
        seg.straight
            ? Array.from({ length: seg.colTo - seg.colFrom + 1 }, (_, i) => ({
                row: seg.row, col: seg.colFrom + i, ids: ['l', 'r'],
              }))
            : [seg]
    );

// Color the given path IDs in a specific grid cell
const applyRoute = (panelId, route, color) => {
    route.forEach(({ row, col, ids }) => {
        const svg = document.querySelector(`#${panelId} [data-row="${row}"][data-col="${col}"]`);
        if (!svg) return;
        ids.forEach(id => {
            const path = svg.querySelector(`[data-id="${id}"]`);
            if (path && path.style.opacity !== '0') path.style.fill = color;
        });
    });
};

// Derive the visual grid automatically from the union of all track cells per coordinate
const buildGrid = zones => {
    const cellMap = new Map(); // "row,col" -> Set of path IDs
    for (const zone of zones) {
        for (const track of Object.values(zone.tracks)) {
            for (const { row, col, ids } of expandRoute(track.cells)) {
                const key = `${row},${col}`;
                if (!cellMap.has(key)) cellMap.set(key, new Set());
                ids.forEach(id => cellMap.get(key).add(id));
            }
        }
    }
    let maxRow = 0, maxCol = 0;
    for (const [key] of cellMap) {
        const [r, c] = key.split(',').map(Number);
        maxRow = Math.max(maxRow, r);
        maxCol = Math.max(maxCol, c);
    }
    const grid = Array.from({ length: maxRow + 1 }, () => Array(maxCol + 1).fill(null));
    for (const [key, ids] of cellMap) {
        const [r, c] = key.split(',').map(Number);
        grid[r][c] = [...ids];
    }
    return grid;
};

// Render SVG tiles for every grid cell
const renderLayout = async (containerId, svgUrl, grid) => {
    const container = document.getElementById(containerId);
    const svgText = await fetch(svgUrl).then(r => r.text());
    const templateSvg = new DOMParser()
        .parseFromString(svgText, 'image/svg+xml')
        .querySelector('svg');

    let segmentIndex = 0;
    for (let row = 0; row < grid.length; row++) {
        for (let col = 0; col < grid[row].length; col++) {
            const cell = grid[row][col];
            if (!cell) {
                const empty = document.createElement('div');
                empty.className = 'empty';
                container.appendChild(empty);
                continue;
            }
            const svg = templateSvg.cloneNode(true);
            svg.setAttribute('data-segment', segmentIndex);
            svg.setAttribute('data-row', row);
            svg.setAttribute('data-col', col);
            svg.querySelectorAll('[data-id]').forEach(path => {
                path.style.opacity = cell.includes(path.getAttribute('data-id')) ? '1' : '0';
                path.setAttribute('data-segment', segmentIndex);
            });
            container.appendChild(svg);
            segmentIndex++;
        }
    }
};

// Update all track colors based on current Arduino state
const updateColors = (tracks, zones) => {
    const interactive = zones.filter(z => z.interactive);
    const passive     = zones.filter(z => !z.interactive);

    // 1. Reset all interactive track cells to inactive
    for (const zone of interactive) {
        for (const trackDef of Object.values(zone.tracks)) {
            expandRoute(trackDef.cells).forEach(({ row, col }) => {
                const svg = document.querySelector(`#layoutPanel [data-row="${row}"][data-col="${col}"]`);
                if (!svg) return;
                svg.querySelectorAll('[data-id]').forEach(path => {
                    if (path.getAttribute('data-id') === 'dot') { path.style.opacity = '0'; return; }
                    if (path.style.opacity !== '0') path.style.fill = COLOR.inactive;
                });
            });
        }
    }

    // 2. Color interactive tracks by Arduino state (selected / occupied)
    for (const zone of interactive) {
        for (const [name, trackDef] of Object.entries(zone.tracks)) {
            const state = tracks[name];
            if (!state) continue;
            if (state.selected) {
                applyRoute('layoutPanel', expandRoute(trackDef.cells), COLOR.selected);
            }
            if (state.occupied) {
                applyRoute('layoutPanel', expandRoute(trackDef.body), COLOR.occupied);
                expandRoute(trackDef.body).forEach(({ row, col }) => {
                    const dot = document.querySelector(
                        `#layoutPanel [data-row="${row}"][data-col="${col}"] [data-id="dot"]`
                    );
                    if (dot) { dot.style.fill = COLOR.occupied; dot.style.opacity = '1'; }
                });
            }
        }
    }

    // 3. Always color non-interactive tracks (frame, zone B, zone C)
    for (const zone of passive) {
        for (const trackDef of Object.values(zone.tracks)) {
            applyRoute('layoutPanel', expandRoute(trackDef.cells), COLOR.selected);
        }
    }
};

let trackConfig = null;

const updateTrackColors = tracks => {
    if (!trackConfig) return;
    updateColors(tracks, trackConfig.zones);
};

fetch('trackConfig.json')
    .then(r => r.json())
    .then(async config => {
        trackConfig = config;
        const grid = buildGrid(config.zones);
        await renderLayout('layoutPanel', 'segment.svg', grid);
        updateTrackColors({});
    });
