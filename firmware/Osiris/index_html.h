#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<title>OSIRIS ESP32-S3</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link href="https://fonts.googleapis.com/css2?family=Oswald:wght@500;600;700&family=Space+Mono:ital,wght@0,400;0,700;1,400&display=swap" rel="stylesheet">
<style>
  :root{
    --bg: #101012; --panel: #17171a; --panel-raised: #1d1d21;
    --cue: #efe6d3; --cue-ink: #201c14;
    --signal: #e2394a; --tele: #39d97a; --tele-dim: #1f7a45;
    --amber: #c98a3b; --muted: #8b8a92; --ink: #eae7dd; --line: #2a2a2f;
    --radius: 14px; --shadow-color: rgba(0,0,0,0.5);
  }
  body.light-theme{
    --bg: #f4f4f8; --panel: #ffffff; --panel-raised: #f9f9fc;
    --cue: #fcf9f2; --cue-ink: #201c14;
    --signal: #d93040; --tele: #1f9d55; --tele-dim: #84d6a5;
    --amber: #b87118; --muted: #6b6a72; --ink: #101012; --line: #d1d1d6;
    --shadow-color: rgba(0,0,0,0.08);
  }
  *{ box-sizing:border-box; }
  html,body{ margin:0; padding:0; transition: background .3s ease, color .3s ease; }
  body{
    background: radial-gradient(1200px 500px at 50% -10%, var(--panel-raised) 0%, var(--bg) 60%);
    color: var(--ink); font-family:'Space Mono', monospace; min-height:100vh;
    padding: 28px 16px 0; overflow-x:hidden;
  }
  .display{ font-family:'Oswald', sans-serif; text-transform:uppercase; letter-spacing:.04em; margin:0; }
  .app{ max-width: 800px; margin:0 auto; padding-bottom: calc(76px + env(safe-area-inset-bottom)); }

  /* ---------- Header (matches the dictionary sample) ---------- */
  .header{ display:flex; align-items:center; justify-content:space-between; margin-bottom:22px; flex-wrap:wrap; gap:12px; }
  .brand{ display:flex; align-items:center; gap:10px; }
  .status-dot{ width:10px; height:10px; border-radius:50%; display:inline-block; flex-shrink:0; box-shadow:0 0 6px currentColor; }
  .status-dot.online{ background: var(--tele); color: var(--tele); }
  .status-dot.offline{ background: var(--signal); color: var(--signal); }
  .brand-text h1{ font-size:24px; font-weight:700; color: var(--ink); line-height:1.1; }
  .brand-text .caption{ font-size:10px; color: var(--muted); letter-spacing:.05em; }
  .header-actions{ display:flex; align-items:center; gap:8px; flex-wrap:wrap; }
  .icon-btn{
    background: var(--panel); border:1px solid var(--line); color: var(--ink);
    width:36px; height:36px; border-radius:10px; display:flex; align-items:center; justify-content:center;
    cursor:pointer; transition: all .15s ease; font-family:'Oswald',sans-serif; font-size:14px; font-weight:600;
  }
  .icon-btn:hover{ border-color: var(--amber); }
  .icon-btn:active{ transform: scale(.94); }
  .icon-svg{ display:block; flex-shrink:0; }
  #langToggle{ font-size:10px; letter-spacing:.04em; }
  .action-btn{
    background: var(--panel); border:1px solid var(--line); color: var(--ink);
    padding:8px 14px; border-radius:8px; font-family:'Oswald',sans-serif; letter-spacing:.05em;
    text-transform:uppercase; font-size:12px; cursor:pointer; transition: all .15s ease;
    display:inline-flex; align-items:center; gap:7px;
  }
  .action-btn:hover{ border-color: var(--tele); color: var(--tele); }
  .action-btn.danger:hover{ border-color: var(--signal); color: var(--signal); }
  .action-btn:disabled{ opacity:.45; cursor:not-allowed; }

  .section{ margin-bottom:22px; }
  .slug{
    display:flex; align-items:center; justify-content:space-between;
    font-size:11px; letter-spacing:.12em; color: var(--amber); text-transform:uppercase; margin-bottom:8px;
  }

  /* ---------- Unsupported-browser / connection banners ---------- */
  .conn-banner{
    background: var(--panel); border:1px solid var(--line); border-left:3px solid var(--signal);
    border-radius:8px; padding:10px 14px; font-size:12px; color: var(--muted); margin-bottom:18px; display:none;
  }
  .conn-banner.show{ display:block; }
  .conn-banner a{ color: var(--amber); }

  /* ---------- Toggle switch ---------- */
  .toggle-switch{ position:relative; width:42px; height:23px; flex-shrink:0; display:inline-block; }
  .toggle-switch input{ opacity:0; width:100%; height:100%; position:absolute; margin:0; cursor:pointer; z-index:1; }
  .toggle-track{ position:absolute; inset:0; background: var(--line); border-radius:999px; transition: background .2s ease; }
  .toggle-thumb{
    position:absolute; top:3px; left:3px; width:17px; height:17px; border-radius:50%;
    background: var(--panel-raised); box-shadow:0 1px 3px rgba(0,0,0,.4); transition: transform .2s ease, background .2s ease;
  }
  .toggle-switch input:checked + .toggle-track{ background: var(--tele-dim); }
  .toggle-switch input:checked + .toggle-track .toggle-thumb{ transform: translateX(19px); background: var(--tele); }
  .toggle-switch input:disabled + .toggle-track{ opacity:.5; cursor:not-allowed; }

  /* ---------- Modes drawer (same left-slide pattern as the sample's history panel) ---------- */
  .scrim{ position:fixed; inset:0; background: var(--shadow-color); opacity:0; pointer-events:none; transition: opacity .2s ease; z-index:40; }
  .scrim.show{ opacity:1; pointer-events:auto; }
  .modes-panel{
    position:fixed; top:0; left:0; bottom:0; width:min(360px, 88vw); background: var(--panel);
    border-right:1px solid var(--line); transform: translateX(-100%); transition: transform .25s ease, background .3s ease;
    z-index:50; display:flex; flex-direction:column;
  }
  .modes-panel.open{ transform: translateX(0); }
  .modes-panel-head{ display:flex; align-items:center; justify-content:space-between; padding:16px; border-bottom:1px solid var(--line); flex-shrink:0; }
  .modes-panel-head h2{ margin:0; font-family:'Oswald',sans-serif; text-transform:uppercase; letter-spacing:.06em; font-size:16px; color: var(--ink); }
  .modes-panel-body{ flex:1; overflow-y:auto; padding:14px 16px 24px; scrollbar-width:thin; }

  /* ---------- Modes panel ---------- */
  .modes-grid{ display:flex; flex-direction:column; background: var(--panel-raised); border:1px solid var(--line); border-radius: var(--radius); padding:2px 16px; }
  .mode-card{ display:flex; align-items:center; justify-content:space-between; padding:12px 0; border-bottom:1px solid var(--line); gap:12px; }
  .mode-card:last-child{ border-bottom:none; }
  .mode-label{ font-family:'Oswald',sans-serif; font-size:14px; letter-spacing:.02em; }
  .mode-sub{ font-size:10px; color: var(--muted); margin-top:2px; }

  /* ---------- Detail sections (greyed until the mode is on; live on the
     main page now, gated by the mode toggles in the left panel) ---------- */
  .detail-section{ background: var(--panel); border:1px solid var(--line); border-radius: var(--radius); padding:14px 16px; margin-top:10px; transition: opacity .2s ease; }
  .detail-section.is-off{ opacity:.42; pointer-events:none; }
  .detail-section h4{
    margin:0 0 10px; font-family:'Oswald',sans-serif; font-size:11.5px; letter-spacing:.08em;
    text-transform:uppercase; color: var(--amber); display:flex; align-items:center; justify-content:space-between;
  }
  .detail-row{ display:flex; align-items:center; justify-content:space-between; gap:10px; padding:6px 0; font-size:12.5px; flex-wrap:wrap; }
  .detail-row + .detail-row{ border-top:1px dashed var(--line); }
  .detail-row .hint{ color: var(--muted); font-size:10.5px; }
  .detail-row input[type=text]{
    background: var(--panel-raised); border:1px solid var(--line); color: var(--ink); border-radius:6px;
    padding:6px 8px; font-family:'Space Mono',monospace; font-size:12px; width:150px;
  }
  .vault-controls{ display:flex; align-items:center; gap:6px; flex-wrap:wrap; justify-content:flex-end; }
  .mini-btn{
    background:none; border:1px dashed var(--line); color: var(--muted); border-radius:6px; padding:4px 8px;
    font-size:10.5px; cursor:pointer; text-transform:uppercase; letter-spacing:.04em; transition: all .15s ease;
  }
  .mini-btn:hover{ border-style:solid; border-color: var(--amber); color: var(--amber); }

  .points-row{ display:flex; gap:6px; flex-wrap:wrap; }
  .slider-point{
    background:none; border:1px solid var(--line); padding:4px 9px; font-family:'Space Mono',monospace;
    font-size:10.5px; letter-spacing:.03em; text-transform:uppercase; color: var(--muted); cursor:pointer;
    border-radius:999px; transition: all .15s ease;
  }
  .slider-point:hover{ color: var(--ink); border-color: var(--amber); }
  .slider-point.active{ color: var(--tele); border-color: var(--tele); font-weight:700; }

  .placeholder-note{ font-size:11px; color: var(--muted); font-style:italic; padding:6px 0; }

  /* ---------- Remote Keyboard ---------- */
  .kb-textarea{
    width:100%; background: var(--panel-raised); border:1px solid var(--line); color: var(--ink);
    border-radius:6px; padding:8px; font-family:'Space Mono',monospace; font-size:12px;
    resize:vertical; min-height:64px; outline:none;
  }
  .kb-textarea:focus{ border-color: var(--tele); }
  .kb-actions{ display:flex; gap:8px; justify-content:flex-end; margin-top:8px; }

  /* ---------- Macros ---------- */
  .macros-grid{ display:grid; grid-template-columns:1fr 1fr; gap:16px; }
  @media (max-width:640px){ .macros-grid{ grid-template-columns:1fr; } }
  .macro-col{ background: var(--panel); border:1px solid var(--line); border-radius: var(--radius); padding:14px; }
  .macro-col-head{ display:flex; align-items:center; justify-content:space-between; font-family:'Oswald',sans-serif; font-size:11.5px; letter-spacing:.08em; text-transform:uppercase; color: var(--amber); margin-bottom:10px; }
  .macro-add-btn{ background:none; border:1px dashed var(--line); color: var(--muted); width:24px; height:24px; border-radius:6px; cursor:pointer; font-size:16px; line-height:1; transition: all .15s ease; }
  .macro-add-btn:hover{ border-color: var(--amber); color: var(--amber); }
  .macro-list{ display:flex; flex-direction:column; gap:8px; }
  .macro-empty{ font-size:11px; color: var(--muted); font-style:italic; }
  .macro-card{
    display:flex; align-items:center; justify-content:space-between; gap:8px; background: var(--panel-raised);
    border:1px solid var(--line); border-radius:8px; padding:8px 10px; cursor:pointer; transition: border-color .15s ease;
  }
  .macro-card:hover{ border-color: var(--tele); }
  .macro-card-text{ min-width:0; }
  .macro-name{ font-size:12px; font-weight:700; }
  .macro-detail{ font-size:10px; color: var(--muted); font-family:'Space Mono',monospace; white-space:nowrap; overflow:hidden; text-overflow:ellipsis; max-width:170px; }
  .macro-del{ background:none; border:none; color: var(--signal); cursor:pointer; font-size:13px; padding:2px 5px; flex-shrink:0; }
  .macro-form{ display:none; flex-direction:column; gap:8px; margin-bottom:10px; background: var(--panel-raised); border:1px solid var(--line); border-radius:8px; padding:10px; }
  .macro-form.show{ display:flex; }
  .macro-form input, .macro-form textarea{
    background: var(--panel); border:1px solid var(--line); color: var(--ink); border-radius:6px;
    padding:7px 9px; font-family:'Space Mono',monospace; font-size:12px; outline:none;
  }
  .macro-form textarea{ resize:vertical; min-height:56px; }
  .macro-form-actions{ display:flex; gap:8px; justify-content:flex-end; }

  /* ---------- Touchpad ---------- */
  .touchpad-area{
    width:100%; height:220px; background: var(--panel-raised);
    border:1px dashed var(--line); border-radius:8px;
    display:flex; align-items:center; justify-content:center;
    touch-action:none; margin-bottom:8px; user-select:none;
    cursor:crosshair; transition:background .15s;
    font-size:12px; color:var(--muted);
  }
  .touchpad-area.active{ background: var(--panel); }
  .mouse-buttons{ display:flex; gap:8px; width:100%; }
  .mouse-btn{
    flex:1; padding:12px 8px; background:var(--panel-raised);
    border:1px solid var(--line); border-radius:8px; color:var(--ink);
    font-family:'Space Mono',monospace; font-size:12px; font-weight:700;
    cursor:pointer; transition:all .15s; text-align:center;
  }
  .mouse-btn:active, .mouse-btn:hover{ border-color:var(--amber); color:var(--amber); }

  /* ---------- Password Vault ---------- */
  .vault-entry{ display:flex; align-items:center; gap:8px; padding:8px 0; border-bottom:1px dashed var(--line); flex-wrap:wrap; }
  .vault-entry:last-child{ border-bottom:none; }
  .vault-name{ font-weight:700; font-size:12px; flex:1; min-width:80px; }
  .vault-user{ font-size:11px; color:var(--muted); font-family:'Space Mono',monospace; }
  .vault-form{ display:none; flex-direction:column; gap:8px; padding:10px; background:var(--panel-raised); border:1px solid var(--line); border-radius:8px; margin-bottom:10px; }
  .vault-form.show{ display:flex; }
  .vault-form input{ background:var(--panel); border:1px solid var(--line); color:var(--ink); border-radius:6px; padding:7px 9px; font-size:12px; font-family:'Space Mono',monospace; width:100%; }
  .vault-form-actions{ display:flex; gap:8px; justify-content:flex-end; }

  /* ---------- Device info badges ---------- */
  .device-badge{ display:inline-block; font-size:10px; font-family:'Space Mono',monospace; background:var(--panel-raised); border:1px solid var(--line); border-radius:5px; padding:2px 7px; color:var(--muted); }

  /* ---------- Settings popup (draggable, same pattern as the sample) ---------- */
  .settings-modal{
    position:fixed; top:12%; left:50%; transform:translateX(-50%); width:320px; max-width:90vw;
    background: var(--panel-raised); border:1px solid var(--line); border-radius: var(--radius);
    box-shadow:0 10px 40px var(--shadow-color); z-index:100; display:none; flex-direction:column; overflow:hidden;
  }
  .settings-modal.show{ display:flex; }
  .settings-header{ display:flex; align-items:center; justify-content:space-between; padding:14px 16px; border-bottom:1px solid var(--line); cursor:grab; background: var(--panel); }
  .settings-header:active{ cursor:grabbing; }
  .settings-header h3{ margin:0; display:flex; align-items:center; gap:8px; font-family:'Oswald',sans-serif; font-size:13px; color: var(--ink); text-transform:uppercase; }
  .settings-body{ padding:16px; display:flex; flex-direction:column; gap:16px; }
  .settings-section label{ font-size:11px; color: var(--amber); text-transform:uppercase; letter-spacing:.05em; display:block; margin-bottom:8px; }
  .settings-section select, .settings-section input[type=range]{
    width:100%; background: var(--panel); border:1px solid var(--line); color: var(--ink); border-radius:8px;
    padding:7px 10px; font-family:'Space Mono',monospace; font-size:12px;
  }
  input[type=range]{ accent-color: var(--tele); cursor:pointer; }

  .footer{
    position: fixed; left:0; right:0; bottom:0; z-index:30;
    background: var(--panel); border-top:1px solid var(--line);
    text-align:center; padding: 12px 16px calc(12px + env(safe-area-inset-bottom));
    font-size:11px; color: var(--muted); letter-spacing:.06em; text-transform:uppercase;
  }

  /* ---------- Mobile adaptations ---------- */
  @media (max-width: 480px){
    body{ padding: 18px 12px 0; }
    .brand-text h1{ font-size:19px; }
    .brand-text .caption{ font-size:9px; }
    .header{ gap:8px; }
    .header-actions{ gap:6px; }
    .icon-btn{ width:34px; height:34px; }
    .action-btn{ padding:7px 11px; font-size:11px; }
    .detail-row{ align-items:flex-start; }
    .detail-row input[type=text]{ width:120px; }
    .vault-controls{ justify-content:flex-start; width:100%; }
    .macro-detail{ max-width:120px; }
    .settings-modal{ width:92vw; top:8%; }
    .modes-panel{ width:92vw; }
    .footer{ font-size:10px; padding: 10px 12px calc(10px + env(safe-area-inset-bottom)); }
    .app{ padding-bottom: calc(68px + env(safe-area-inset-bottom)); }
  }
</style>
</head>
<body class="dark-theme" id="appBody">

<div class="scrim" id="scrim"></div>

<!-- Modes drawer (left slide-in panel) -->
<aside class="modes-panel" id="modesPanel">
  <div class="modes-panel-head">
    <h2 data-i18n="modesTitle">Modes</h2>
    <button class="icon-btn" id="closeModes" style="width:28px;height:28px;"><svg class="icon-svg" width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M18 6L6 18M6 6l12 12"/></svg></button>
  </div>
  <div class="modes-panel-body">
    <div class="modes-grid" id="modesGrid">
      <div class="mode-card">
        <div>
          <div class="mode-label">Mouse Jiggler</div>
          <div class="mode-sub" data-i18n="subJig">Empêche la mise en veille du PC</div>
        </div>
        <label class="toggle-switch"><input type="checkbox" id="toggle-jig" data-mode="JIG"><div class="toggle-track"><div class="toggle-thumb"></div></div></label>
      </div>
      <div class="mode-card">
        <div>
          <div class="mode-label">AP Mode (Access Point) PC</div>
          <div class="mode-sub" data-i18n="subAp">Le PC devient un point d'accès</div>
        </div>
        <label class="toggle-switch"><input type="checkbox" id="toggle-ap" data-mode="AP"><div class="toggle-track"><div class="toggle-thumb"></div></div></label>
      </div>
      <div class="mode-card">
        <div>
          <div class="mode-label">WIFI Mode (Android)</div>
          <div class="mode-sub" data-i18n="subWifi">Connexion à un réseau existant</div>
        </div>
        <label class="toggle-switch"><input type="checkbox" id="toggle-wifi" data-mode="WIFI"><div class="toggle-track"><div class="toggle-thumb"></div></div></label>
      </div>
      <div class="mode-card">
        <div>
          <div class="mode-label">BLE (Bluetooth) Mode</div>
          <div class="mode-sub" data-i18n="subBle">Diffusion Bluetooth Low Energy</div>
        </div>
        <label class="toggle-switch"><input type="checkbox" id="toggle-ble" data-mode="BLE"><div class="toggle-track"><div class="toggle-thumb"></div></div></label>
      </div>
    </div>
  </div>
</aside>

<!-- Device parameters popup: brightness / screen timeout / menu mode -->
<div class="settings-modal" id="paramsModal">
  <div class="settings-header" id="paramsHeader">
    <h3>
      <svg width="15px" height="15px" viewBox="0 0 24 24" fill="none" stroke="currentColor">
<path d="M18.9401 5.42141L13.7701 2.43141C12.7801 1.86141 11.2301 1.86141 10.2401 2.43141L5.02008 5.44141C2.95008 6.84141 2.83008 7.05141 2.83008 9.28141V14.7114C2.83008 16.9414 2.95008 17.1614 5.06008 18.5814L10.2301 21.5714C10.7301 21.8614 11.3701 22.0014 12.0001 22.0014C12.6301 22.0014 13.2701 21.8614 13.7601 21.5714L18.9801 18.5614C21.0501 17.1614 21.1701 16.9514 21.1701 14.7214V9.28141C21.1701 7.05141 21.0501 6.84141 18.9401 5.42141ZM12.0001 15.2514C10.2101 15.2514 8.75008 13.7914 8.75008 12.0014C8.75008 10.2114 10.2101 8.75141 12.0001 8.75141C13.7901 8.75141 15.2501 10.2114 15.2501 12.0014C15.2501 13.7914 13.7901 15.2514 12.0001 15.2514Z" fill="currentColor"/>
</svg><span data-i18n="settingsTitle">Paramètres</span>
    </h3>
    <button class="icon-btn" id="closeParams" style="width:32px;height:32px;background:var(--signal);color:#fff;border:none;"><svg class="icon-svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"><path d="M18 6L6 18M6 6l12 12"/></svg></button>
  </div>
  <div class="settings-body">
    <div class="settings-section">
      <label><span data-i18n="brightness">Luminosité</span> — <span id="brightVal">255</span></label>
      <input type="range" id="brightnessSlider" min="0" max="255" value="255">
    </div>
    <div class="settings-section">
      <label data-i18n="screenTimeout">Mise en veille de l'écran</label>
      <select id="timeoutSelect">
        <option value="0" data-i18n="never">Jamais</option>
        <option value="15">15 s</option>
        <option value="30">30 s</option>
        <option value="60">60 s</option>
        <option value="120">120 s</option>
      </select>
    </div>
    <div class="settings-section">
      <label data-i18n="menuMode">Mode menu</label>
      <div class="points-row" id="menuModePoints">
        <button type="button" class="slider-point active" data-mode="LIST" data-i18n="list">Liste</button>
        <button type="button" class="slider-point" data-mode="GRID" data-i18n="grid">Grille</button>
      </div>
    </div>
  </div>
</div>

<div class="app">
  <header class="header">
    <div class="brand">
      <span id="deviceStatus" class="status-dot offline" title="Déconnecté"></span>
      <div class="brand-text">
        <h1 class="display">Osiris ESP32-S3</h1>
        <div class="caption"><span id="deviceCaption">Jig de contrôle · déconnecté</span> <span id="devTemp"></span></div>
      </div>
    </div>
    <div class="header-actions">
      <button class="icon-btn" id="openModes" title="Modes">
        <svg class="icon-svg" width="20" height="20" viewBox="0 0 128 128" fill="none"><g clip-path="url(#clip0)">
  <path d="m72.235 79.21c0.8345-2.722 1.1439-4.4416 0.7211-7.2584-0.4613-3.0734-1.0598-5.062-3.2451-7.2583-2.461-2.4734-9.0143-3.2663-9.0143-3.2663s0 7.858-2.1634 8.7101c-1.0821 0.4261-7.0109-4.287-7.2115-8.7101-0.0407-0.8972 6.0251-8.7277 11.899-9.4359 1.4419-0.1739 14.945 6.8788 16.226 15.606 0.7836 5.339-0.4864 8.8003-3.2451 13.428-2.1634 3.6292-23.798 20.687-23.798 20.687s3.0494 1.895 10.817 1.088c7.4974-0.778 19.686-4.2359 21.634-5.4435 3.966-2.4587 18.029-21.049 19.11-34.114 1.082-13.065-23.437-41.976-29.927-43.187-4.3269-0.8079-30.649 7.5406-35.336 11.976-2.6845 2.5404-13.702 22.501-13.702 26.856 0 2.1775 7.2114 19.477 10.096 22.138 2.8846 2.6615 23.438 11.062 25.601 10.162 2.524-1.05 10.704-9.2543 11.538-11.976z" fill="currentColor"/>
 </g>
 <defs>
  <clipPath id="clip0">
   <rect width="128" height="128" fill="#ffffff"/>
  </clipPath>
 </defs>
</svg>
      </button>
      <button class="icon-btn" id="langToggle" title="Langue (FR/EN)"><svg width="18" height="18" viewBox="0 0 24 24" role="img" aria-labelledby="languageIconTitle" stroke="currentColor" stroke-width="1.5" stroke-linecap="square" stroke-linejoin="miter" fill="none" color="currentColor"> <title id="languageIconTitle">Language</title> <circle cx="12" cy="12" r="10"/> <path stroke-linecap="round" d="M12,22 C14.6666667,19.5757576 16,16.2424242 16,12 C16,7.75757576 14.6666667,4.42424242 12,2 C9.33333333,4.42424242 8,7.75757576 8,12 C8,16.2424242 9.33333333,19.5757576 12,22 Z"/> <path stroke-linecap="round" d="M2.5 9L21.5 9M2.5 15L21.5 15"/> </svg><span id="langToggleText">FR</span></button>
      <button class="icon-btn" id="themeToggle" title="Thème clair/sombre"><svg width="18" height="18" fill="currentColor" viewBox="0 0 32 32">
<title>contrast</title>
<path d="M0 16q0 3.264 1.28 6.24t3.392 5.088 5.12 3.424 6.208 1.248q3.264 0 6.24-1.248t5.088-3.424 3.392-5.088 1.28-6.24-1.28-6.208-3.392-5.12-5.088-3.392-6.24-1.28q-3.264 0-6.208 1.28t-5.12 3.392-3.392 5.12-1.28 6.208zM4 16q0-3.264 1.6-6.016t4.384-4.352 6.016-1.632 6.016 1.632 4.384 4.352 1.6 6.016-1.6 6.048-4.384 4.352-6.016 1.6-6.016-1.6-4.384-4.352-1.6-6.048zM16 26.016q2.72 0 5.024-1.344 t3.648-3.648 1.344-5.024-1.344-4.992-3.648-3.648-5.024-1.344v20z"></path>
</svg></button>
      <button class="icon-btn" id="openParams" title="Paramètres">
       <svg width="18" height="18" viewBox="0 0 24 24" fill="none">
<path d="M18.9401 5.42141L13.7701 2.43141C12.7801 1.86141 11.2301 1.86141 10.2401 2.43141L5.02008 5.44141C2.95008 6.84141 2.83008 7.05141 2.83008 9.28141V14.7114C2.83008 16.9414 2.95008 17.1614 5.06008 18.5814L10.2301 21.5714C10.7301 21.8614 11.3701 22.0014 12.0001 22.0014C12.6301 22.0014 13.2701 21.8614 13.7601 21.5714L18.9801 18.5614C21.0501 17.1614 21.1701 16.9514 21.1701 14.7214V9.28141C21.1701 7.05141 21.0501 6.84141 18.9401 5.42141ZM12.0001 15.2514C10.2101 15.2514 8.75008 13.7914 8.75008 12.0014C8.75008 10.2114 10.2101 8.75141 12.0001 8.75141C13.7901 8.75141 15.2501 10.2114 15.2501 12.0014C15.2501 13.7914 13.7901 15.2514 12.0001 15.2514Z" fill="currentColor"/>
</svg>
      </button>
      <button class="action-btn" id="connectBtn">
        <svg class="icon-svg" width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M4 9V5a1 1 0 011-1h1M20 9V5a1 1 0 00-1-1h-1M4 15v4a1 1 0 001 1h1M20 15v4a1 1 0 01-1 1h-1"/><rect x="7" y="9" width="10" height="7" rx="1"/></svg>
        <span id="connectBtnText" data-i18n="connect">Connecter</span>
      </button>
    </div>
  </header>

  <div class="conn-banner" id="unsupportedBanner" data-i18n="unsupported">
    Ce navigateur ne prend pas en charge Web Serial. Utilisez Chrome, Edge ou Opera sur ordinateur pour connecter l'Osiris ESP32-S3.
  </div>
  <div class="conn-banner" id="errorBanner"></div>

  <section class="section">
    <div class="slug"><span data-i18n="detailsTitle">Détails</span></div>

    <div class="detail-section is-off" id="detail-jig">
      <h4 data-i18n="hJig">Mouse Jiggler</h4>
      <div class="detail-row">
        <span data-i18n="durationLabel">Durée (mins)</span>
        <div class="points-row" id="jigTimePoints">
          <button type="button" class="slider-point" data-time="30">30</button>
          <button type="button" class="slider-point" data-time="60">60</button>
          <button type="button" class="slider-point" data-time="90">90</button>
          <button type="button" class="slider-point" data-time="0" data-i18n="unlim">Illim.</button>
        </div>
      </div>
    </div>

    <div class="detail-section is-off" id="detail-ap">
      <h4><span>AP Mode</span> <span class="hint">(<span id="apClientCount">0</span> <span data-i18n="connectedDevices">appareil(s) connecté(s)</span>)</span></h4>
      <div class="detail-row">
        <button type="button" class="mini-btn" data-show-target="ap-devices" data-i18n="showHide">Afficher / Masquer</button>
      </div>
      <div class="placeholder-note" id="ap-devices" style="display:none;" data-i18n="noDevices">Aucun appareil connecté pour le moment.</div>
    </div>

    <div class="detail-section is-off" id="detail-wifi">
      <h4 data-i18n="hWifi">WIFI Mode</h4>
      <div class="detail-row">
        <span data-i18n="connectedTo">Connecté à</span>
        <span class="hint" id="wifiConnectedTo">—</span>
      </div>
      <div class="detail-row">
        <span data-i18n="networkLabel">Réseau</span>
        <span class="vault-controls">
          <input type="text" id="wifiSsidInput" placeholder="SSID" data-i18n-placeholder="ssidPh">
          <input type="password" id="wifiPassInput" placeholder="Mot de passe" data-i18n-placeholder="wifiPassPh">
          <button type="button" class="mini-btn" id="wifiSaveBtn" data-i18n="saveConnect">Enregistrer</button>
        </span>
      </div>
      <div class="detail-row">
        <button type="button" class="mini-btn" id="wifiScanBtn" data-i18n="scanNetworks">Scanner</button>
      </div>
      <div class="placeholder-note" id="wifiScanList" style="display:none;"></div>
    </div>

    <div class="detail-section is-off" id="detail-ble">
      <h4><span>BLE Mode</span> <span class="hint">(<span id="bleConnStatus" data-i18n="noDevices">non connecté</span>)</span></h4>
      <div class="detail-row">
        <button type="button" class="mini-btn" data-show-target="ble-devices" data-i18n="showHide">Afficher / Masquer</button>
      </div>
      <div class="placeholder-note" id="ble-devices" style="display:none;" data-i18n="noDevices">Aucun appareil connecté pour le moment.</div>
    </div>
  </section>

  <section class="section">
    <div class="slug"><span data-i18n="remoteKbTitle">Clavier distant</span></div>
    <div class="detail-section is-off" id="detail-kb">
      <h4 data-i18n="remoteKbSubtitle">Taper à distance</h4>
      <div class="placeholder-note" id="kbLayoutNote"></div>
      <!-- F-key row -->
      <div class="detail-row" style="flex-wrap:wrap; gap:4px;">
        <button type="button" class="slider-point" data-fkey="F1">F1</button>
        <button type="button" class="slider-point" data-fkey="F2">F2</button>
        <button type="button" class="slider-point" data-fkey="F3">F3</button>
        <button type="button" class="slider-point" data-fkey="F4">F4</button>
        <button type="button" class="slider-point" data-fkey="F5">F5</button>
        <button type="button" class="slider-point" data-fkey="F6">F6</button>
        <button type="button" class="slider-point" data-fkey="F7">F7</button>
        <button type="button" class="slider-point" data-fkey="F8">F8</button>
        <button type="button" class="slider-point" data-fkey="F9">F9</button>
        <button type="button" class="slider-point" data-fkey="F10">F10</button>
        <button type="button" class="slider-point" data-fkey="F11">F11</button>
        <button type="button" class="slider-point" data-fkey="F12">F12</button>
      </div>
      <div class="detail-row" style="flex-direction:column; align-items:stretch;">
        <textarea class="kb-textarea" id="kbText" rows="3" data-i18n-placeholder="remoteKbPh" placeholder="Tapez ici… envoyé sur le clavier actif — USB si branché, sinon BLE (Ctrl+Entrée pour envoyer)."></textarea>
        <div class="kb-actions">
          <button type="button" class="mini-btn" id="kbClear" data-i18n="kbClearBtn">Effacer</button>
          <button type="button" class="mini-btn" id="kbSend" data-i18n="kbSendBtn">Envoyer (Ctrl+Entrée)</button>
        </div>
      </div>
      <div class="detail-row">
        <div class="points-row" id="kbSpecialKeys">
          <button type="button" class="slider-point" data-macro="ENTER" data-i18n="keyEnter">Entrée</button>
          <button type="button" class="slider-point" data-macro="TAB">Tab</button>
          <button type="button" class="slider-point" data-macro="BACKSPACE" data-i18n="keyBackspace">Retour</button>
          <button type="button" class="slider-point" data-macro="ESC">Esc</button>
          <button type="button" class="slider-point" data-macro="SPACE">Espace</button>
          <button type="button" class="slider-point" data-macro="UP">↑</button>
          <button type="button" class="slider-point" data-macro="DOWN">↓</button>
          <button type="button" class="slider-point" data-macro="LEFT">←</button>
          <button type="button" class="slider-point" data-macro="RIGHT">→</button>
          <button type="button" class="slider-point" data-macro="CTRL+C">Ctrl+C</button>
          <button type="button" class="slider-point" data-macro="CTRL+V">Ctrl+V</button>
          <button type="button" class="slider-point" data-macro="CTRL+Z">Ctrl+Z</button>
          <button type="button" class="slider-point" data-macro="CTRL+A">Ctrl+A</button>
          <button type="button" class="slider-point" data-macro="CTRL+S">Ctrl+S</button>
          <button type="button" class="slider-point" data-macro="ALT+TAB">Alt+Tab</button>
          <button type="button" class="slider-point" data-macro="CTRL+ALT+DELETE">Ctrl+Alt+Del</button>
        </div>
      </div>
      <div class="placeholder-note" id="kbTargetNote" data-i18n="kbTargetUsb">Clavier actif : USB (câblé).</div>
    </div>
  </section>

  <section class="section">
    <div class="slug">
      <span data-i18n="touchpadTitle">Pavé tactile virtuel</span>
      <button type="button" class="mini-btn" id="touchpadToggle" style="margin-left:auto;" data-i18n="showHide">Afficher</button>
    </div>
    <div id="touchpadSection" style="display:none;">
      <div class="detail-section is-off" id="detail-touchpad">
        <div id="touchpadArea" class="touchpad-area" data-i18n="touchpadHint">Glisser pour déplacer</div>
        <div class="mouse-buttons">
          <button type="button" class="mouse-btn" id="mouseLeft" data-i18n="mouseL">◀ Gauche</button>
          <button type="button" class="mouse-btn" id="mouseRight" data-i18n="mouseR">Droit ▶</button>
        </div>
      </div>
    </div>
  </section>

  <section class="section">
    <div class="slug"><span data-i18n="vaultTitle">Coffre-fort</span></div>
    <div class="detail-section is-off" id="detail-vault">
      <div class="vault-form" id="vaultForm">
        <input type="text" id="newVaultName" data-i18n-placeholder="vaultNamePh" placeholder="Nom (ex: Gmail)">
        <input type="text" id="newVaultUser" data-i18n-placeholder="vaultUserPh" placeholder="Identifiant">
        <input type="password" id="newVaultPass" data-i18n-placeholder="vaultPassPh" placeholder="Mot de passe">
        <div class="vault-form-actions">
          <button type="button" class="mini-btn" id="vaultCancel" data-i18n="cancel">Annuler</button>
          <button type="button" class="mini-btn" id="vaultSaveBtn" data-i18n="save">Enregistrer</button>
        </div>
      </div>
      <div id="vaultList"></div>
      <button type="button" class="macro-add-btn" id="addVaultBtn" title="Ajouter un mot de passe">+</button>
      <div class="placeholder-note" data-i18n="vaultNote">Les mots de passe sont envoyés directement via HID. Jamais transmis par le réseau.</div>
    </div>
  </section>

</div>

<footer class="footer">By Osiris Inc.</footer>

<script>
(function(){
  "use strict";

  const TRANSLATIONS = {
    fr: {
      modesTitle: 'Modes', subJig: "Empêche la mise en veille du PC",
      subAp: "Le PC devient un point d'accès", subWifi: "Connexion à un réseau existant",
      subBle: "Diffusion Bluetooth Low Energy",
      detailsTitle: 'Détails',
      hJig: 'Mouse Jiggler', durationLabel: 'Durée (mins)', unlim: 'Illim.',
      connectedDevices: 'appareil(s) connecté(s)', showHide: 'Afficher / Masquer',
      noDevices: 'Aucun appareil connecté.', hWifi: 'WIFI Mode',
      connectedTo: 'Connecté à', networkLabel: 'Réseau', ssidPh: 'SSID',
      wifiPassPh: 'Mot de passe', saveConnect: 'Enregistrer',
      connecting: 'connexion...', ssidError: "SSID requis (sans ':').",
      scanNetworks: 'Scanner', scanning: 'Recherche...', noNetworks: 'Aucun réseau.',
      remoteKbTitle: 'Clavier distant', remoteKbSubtitle: 'Taper à distance',
      remoteKbPh: "Tapez ici… (Ctrl+Entrée pour envoyer)",
      kbSendBtn: 'Envoyer', kbClearBtn: 'Effacer',
      keyEnter: 'Entrée', keyBackspace: 'Retour',
      kbTargetUsb: 'Clavier actif : USB.', kbTargetBle: 'Clavier actif : BLE.',
      kbTargetNone: 'Aucun clavier connecté.',
      kbLayoutNotePrefix: 'Disposition cible :',
      touchpadTitle: 'Pavé tactile', touchpadHint: 'Glisser pour déplacer',
      mouseL: '◀ Gauche', mouseR: 'Droit ▶',
      vaultTitle: 'Coffre-fort', vaultNamePh: 'Nom (ex: Gmail)',
      vaultUserPh: 'Identifiant', vaultPassPh: 'Mot de passe',
      vaultNote: 'Injecté via HID — jamais transmis par le réseau.',
      cancel: 'Annuler', save: 'Enregistrer',
      connect: 'Connecter', disconnect: 'Déconnecter',
      connected: 'Connecté', disconnected: 'Déconnecté',
      captionConnected: 'Jig de contrôle · connecté',
      captionDisconnected: 'Jig de contrôle · déconnecté',
      connectError: 'Erreur connexion : ', linkError: 'Lien perdu.', sendError: 'Erreur envoi.',
      unsupported: "Ce navigateur ne prend pas en charge Web Serial. Utilisez Chrome, Edge ou Opera sur ordinateur pour connecter l'Osiris ESP32-S3.",
      vaultFull: "Coffre plein (8 emplacements maximum).",
      vaultBadChars: "Le nom et l'identifiant ne doivent pas contenir '|' ou ';'.",
    },
    en: {
      modesTitle: 'Modes', subJig: "Prevents PC from sleeping",
      subAp: "PC becomes an access point", subWifi: "Join an existing network",
      subBle: "Bluetooth Low Energy broadcast",
      detailsTitle: 'Details',
      hJig: 'Mouse Jiggler', durationLabel: 'Duration (mins)', unlim: 'Unlim.',
      connectedDevices: 'device(s) connected', showHide: 'Show / Hide',
      noDevices: 'No device connected.', hWifi: 'WIFI Mode',
      connectedTo: 'Connected to', networkLabel: 'Network', ssidPh: 'SSID',
      wifiPassPh: 'Password', saveConnect: 'Save',
      connecting: 'connecting...', ssidError: "SSID required (no ':').",
      scanNetworks: 'Scan', scanning: 'Scanning...', noNetworks: 'No networks found.',
      remoteKbTitle: 'Remote Keyboard', remoteKbSubtitle: 'Type remotely',
      remoteKbPh: "Type here… (Ctrl+Enter to send)",
      kbSendBtn: 'Send', kbClearBtn: 'Clear',
      keyEnter: 'Enter', keyBackspace: 'Backspace',
      kbTargetUsb: 'Active keyboard: USB.', kbTargetBle: 'Active keyboard: BLE.',
      kbTargetNone: 'No keyboard connected.',
      kbLayoutNotePrefix: 'Target layout:',
      touchpadTitle: 'Virtual Touchpad', touchpadHint: 'Slide to move',
      mouseL: '◀ Left', mouseR: 'Right ▶',
      vaultTitle: 'Password Vault', vaultNamePh: 'Name (e.g. Gmail)',
      vaultUserPh: 'Username', vaultPassPh: 'Password',
      vaultNote: 'Injected via HID — never transmitted over the network.',
      cancel: 'Cancel', save: 'Save',
      connect: 'Connect', disconnect: 'Disconnect',
      connected: 'Connected', disconnected: 'Disconnected',
      captionConnected: 'Control jig · connected',
      captionDisconnected: 'Control jig · disconnected',
      connectError: 'Connect error: ', linkError: 'Link lost.', sendError: 'Send error.',
      unsupported: "This browser doesn't support Web Serial. Use Chrome, Edge, or Opera on desktop to connect Osiris ESP32-S3.",
      vaultFull: "Vault is full (8 slots maximum).",
      vaultBadChars: "Name and username cannot contain '|' or ';'.",
    },
  };

  let state = JSON.parse(localStorage.getItem('osirisUiState') || localStorage.getItem('phoenixUiState')) || { theme: 'dark', lang: 'fr', customMacros: [], vault: [] };
  function saveUiState() { localStorage.setItem('osirisUiState', JSON.stringify(state)); }
  function tr(key) { return (TRANSLATIONS[state.lang] || TRANSLATIONS.fr)[key] || key; }

  const appBody = document.getElementById('appBody');
  function applyTheme() { appBody.className = state.theme === 'light' ? 'light-theme' : 'dark-theme'; }
  document.getElementById('themeToggle').addEventListener('click', () => {
    state.theme = state.theme === 'light' ? 'dark' : 'light';
    applyTheme(); saveUiState();
  });

  function applyLang() {
    document.getElementById('langToggleText').textContent = state.lang.toUpperCase();
    document.documentElement.lang = state.lang;
    document.querySelectorAll('[data-i18n]').forEach(el => { el.textContent = tr(el.getAttribute('data-i18n')); });
    document.querySelectorAll('[data-i18n-placeholder]').forEach(el => { el.placeholder = tr(el.getAttribute('data-i18n-placeholder')); });
    document.querySelectorAll('[data-i18n-title]').forEach(el => { el.title = tr(el.getAttribute('data-i18n-title')); });
    refreshConnectionUI();
  }
  document.getElementById('langToggle').addEventListener('click', () => {
    state.lang = state.lang === 'fr' ? 'en' : 'fr';
    applyLang(); saveUiState();
  });

  const modesPanel = document.getElementById('modesPanel');
  const scrim = document.getElementById('scrim');
  const openModesFn = () => { modesPanel.classList.add('open'); scrim.classList.add('show'); };
  const closeModesFn = () => { modesPanel.classList.remove('open'); scrim.classList.remove('show'); };
  document.getElementById('openModes').addEventListener('click', openModesFn);
  document.getElementById('closeModes').addEventListener('click', closeModesFn);
  scrim.addEventListener('click', closeModesFn);

  let port = null, reader = null, writer = null, keepReading = false, lineBuffer = "";
  let socket = null;

  const connectBtn = document.getElementById('connectBtn');
  const connectBtnText = document.getElementById('connectBtnText');
  const deviceStatus = document.getElementById('deviceStatus');
  const deviceCaption = document.getElementById('deviceCaption');
  const unsupportedBanner = document.getElementById('unsupportedBanner');
  const wifiConnectedTo = document.getElementById('wifiConnectedTo');
  const wifiSsidInput = document.getElementById('wifiSsidInput');
  const wifiPassInput = document.getElementById('wifiPassInput');
  const wifiScanList = document.getElementById('wifiScanList');
  document.getElementById('wifiSaveBtn').addEventListener('click', () => {
    const ssid = wifiSsidInput.value.trim();
    const pass = wifiPassInput.value;
    if (!ssid || ssid.includes(':')) { showError(tr('ssidError')); return; }
    send('CMD:WIFI:SET:' + ssid + ':' + pass);
    send('CMD:WIFI:ON');
    wifiPassInput.value = '';
  });
  document.getElementById('wifiScanBtn').addEventListener('click', () => {
    wifiScanList.style.display = 'block';
    wifiScanList.textContent = tr('scanning');
    scannedNetworks = [];
    send('CMD:NET:SCAN');
  });
  const errorBanner = document.getElementById('errorBanner');

  const supported = 'serial' in navigator;
  if (!supported) unsupportedBanner.classList.add('show');

  function showError(msg) {
    errorBanner.textContent = msg;
    errorBanner.classList.add('show');
    setTimeout(() => errorBanner.classList.remove('show'), 6000);
  }

  function setConnected(isConnected) {
    deviceStatus.className = 'status-dot ' + (isConnected ? 'online' : 'offline');
    deviceStatus.title = isConnected ? tr('connected') : tr('disconnected');
    deviceCaption.textContent = isConnected ? tr('captionConnected') : tr('captionDisconnected');
    connectBtnText.textContent = port ? tr('disconnect') : tr('connect'); // this button only ever controls the USB/Serial link specifically
    setModesEnabled(isConnected);
  }

  // True if EITHER transport is up — the AP's WebSocket (which auto-connects
  // itself below, no user gesture needed since you're already on this page
  // via the device's own AP) or Web Serial over USB (which still needs the
  // explicit Connect button, since requestPort() requires a user gesture).
  function anyTransportConnected() {
    return !!port || (socket && socket.readyState === WebSocket.OPEN);
  }
  function refreshConnectionUI() { setConnected(anyTransportConnected()); }

  function sendTime() {
    const now = new Date();
    const hh = String(now.getHours()).padStart(2, '0');
    const mm = String(now.getMinutes()).padStart(2, '0');
    const dd = String(now.getDate()).padStart(2, '0');
    const mo = String(now.getMonth() + 1).padStart(2, '0');
    const yy = now.getFullYear();
    send('CMD:TIME:SET:' + hh + ':' + mm + ':' + dd + '/' + mo + '/' + yy);
  }

  function onAfterConnect() {
    send('CMD:STATE?');
    sendTime();
    send('CMD:VAULT:LIST?');
  }

  // Auto-connects to this same device's own /ws endpoint when served over HTTP/AP.
  function connectSocket() {
    if (!location.host) return;
    let ws;
    try {
      ws = new WebSocket('ws://' + location.host + '/ws');
    } catch (e) {
      console.error(e);
      setTimeout(connectSocket, 3000);
      return;
    }
    socket = ws;
    ws.onopen = () => {
      refreshConnectionUI();
      onAfterConnect();
    };
    ws.onmessage = (evt) => {
      String(evt.data).split('\n').forEach(line => {
        line = line.trim();
        if (line) handleLine(line);
      });
    };
    ws.onclose = () => {
      if (socket === ws) socket = null;
      refreshConnectionUI();
      setTimeout(connectSocket, 3000);
    };
    ws.onerror = () => { try { ws.close(); } catch (e) {} };
  }

  async function connect() {
    if (!supported) { unsupportedBanner.classList.add('show'); return; }
    if (port) { await disconnect(); return; }
    try {
      port = await navigator.serial.requestPort();
      await port.open({ baudRate: 115200 });
      writer = port.writable.getWriter();
      keepReading = true;
      readLoop();
      refreshConnectionUI();
      onAfterConnect();
    } catch (e) {
      console.error(e);
      if (e && e.name !== 'NotFoundError') showError(tr('connectError') + (e.message || e));
      port = null;
    }
  }

  async function disconnect() {
    keepReading = false;
    try { if (reader) { await reader.cancel(); } } catch (e) {}
    try { if (writer) { writer.releaseLock(); } } catch (e) {}
    try { if (port) { await port.close(); } } catch (e) {}
    port = null; reader = null; writer = null;
    refreshConnectionUI();
  }

  async function tryAutoReconnect() {
    if (!supported) return;
    try {
      const ports = await navigator.serial.getPorts();
      if (!ports.length) return;
      port = ports[0];
      await port.open({ baudRate: 115200 });
      writer = port.writable.getWriter();
      keepReading = true;
      readLoop();
      refreshConnectionUI();
      onAfterConnect();
    } catch (e) {
      console.error(e);
      port = null;
    }
  }

  async function readLoop() {
    const textDecoder = new TextDecoderStream();
    const closedPromise = port.readable.pipeTo(textDecoder.writable).catch(() => {});
    reader = textDecoder.readable.getReader();
    try {
      while (keepReading) {
        const { value, done } = await reader.read();
        if (done) break;
        lineBuffer += value;
        let idx;
        while ((idx = lineBuffer.indexOf('\n')) >= 0) {
          const line = lineBuffer.slice(0, idx).trim();
          lineBuffer = lineBuffer.slice(idx + 1);
          if (line) handleLine(line);
        }
      }
    } catch (e) {
      console.error(e);
      showError(tr('linkError'));
    } finally {
      try { reader.releaseLock(); } catch (e) {}
      await closedPromise;
      if (keepReading) { keepReading = false; port = null; writer = null; refreshConnectionUI(); }
    }
  }

  async function send(cmdLine) {
    if (socket && socket.readyState === WebSocket.OPEN) {
      try { socket.send(cmdLine); } catch (e) { console.error(e); }
      return;
    }
    if (!writer) return;
    try {
      await writer.write(new TextEncoder().encode(cmdLine + '\n'));
    } catch (e) {
      console.error(e);
      showError(tr('sendError'));
    }
  }

  if (supported && navigator.serial.addEventListener) {
    navigator.serial.addEventListener('disconnect', () => { if (port) disconnect(); });
  }

  connectBtn.addEventListener('click', connect);

  let scannedNetworks = [];

  function handleLine(line) {
    if (line.startsWith('STATE:')) { parseState(line.slice(6)); return; }
    if (line.startsWith('DEVTEMP:')) {
      const c = line.slice(8);
      document.getElementById('devTemp').textContent = c ? ('· ' + c + '°C') : '';
      return;
    }
    if (line.startsWith('NET:ITEM:')) {
      const r = line.slice(9);
      const parts = r.split(':');
      const enc = parts.pop();
      const rssi = parts.pop();
      const ssid = parts.join(':');
      scannedNetworks.push({ ssid, rssi, enc: enc === '1' });
      return;
    }
    if (line.startsWith('NET:SCANDONE')) { renderScanList(); return; }
    if (line.startsWith('NET:COUNT:') || line.startsWith('READY:') || line.startsWith('OK:') || line.startsWith('ERR:')) { console.log('[osiris]', line); return; }
    if (line.startsWith('VAULT:LIST:')) { renderVaultFromFirmware(line.slice(11)); return; }
  }

  function renderScanList() {
    wifiScanList.innerHTML = '';
    if (!scannedNetworks.length) { wifiScanList.textContent = tr('noNetworks'); return; }
    scannedNetworks.forEach(n => {
      const btn = document.createElement('button');
      btn.type = 'button';
      btn.className = 'mini-btn';
      btn.style.display = 'block';
      btn.style.margin = '3px 0';
      btn.textContent = (n.enc ? '🔒 ' : '') + n.ssid;
      btn.addEventListener('click', () => { wifiSsidInput.value = n.ssid; wifiPassInput.focus(); });
      wifiScanList.appendChild(btn);
    });
  }

  function parseState(body) {
    const s = {};
    body.split(';').forEach(pair => {
      const eq = pair.indexOf('=');
      if (eq === -1) return;
      s[pair.slice(0, eq)] = pair.slice(eq + 1);
    });
    setToggle('toggle-jig', s.JIG === 'ON');
    setToggle('toggle-ap', s.AP === 'ON');
    setToggle('toggle-wifi', s.WIFI === 'ON');
    setToggle('toggle-ble', s.BLE === 'ON');

    document.getElementById('detail-jig').classList.toggle('is-off', s.JIG !== 'ON');
    document.getElementById('detail-ap').classList.toggle('is-off', s.AP !== 'ON');
    document.getElementById('detail-ble').classList.toggle('is-off', s.BLE !== 'ON');

    if (s.WIFISSID !== undefined) {
      wifiConnectedTo.textContent = (s.WIFI === 'ON' && s.WIFIIP) ? (s.WIFISSID + ' (' + s.WIFIIP + ')')
                                    : s.WIFISSID ? (s.WIFISSID + ' — ' + tr('connecting'))
                                    : '—';
    }
    if (s.JIGT !== undefined) {
      document.querySelectorAll('#jigTimePoints .slider-point').forEach(pt => {
        pt.classList.toggle('active', pt.getAttribute('data-time') === s.JIGT);
      });
    }
    if (s.BRIGHT !== undefined) {
      brightnessSlider.value = s.BRIGHT;
      brightVal.textContent = s.BRIGHT;
    }
    if (s.TOUT !== undefined) timeoutSelect.value = s.TOUT;
    if (s.MENU !== undefined) {
      document.querySelectorAll('#menuModePoints .slider-point').forEach(pt => {
        pt.classList.toggle('active', pt.getAttribute('data-mode') === s.MENU);
      });
    }
    if (s.USB !== undefined || s.BLECONN !== undefined) {
      const note = document.getElementById('kbTargetNote');
      if (note) note.textContent = s.USB === 'ON' ? tr('kbTargetUsb')
                                  : s.BLECONN === 'ON' ? tr('kbTargetBle')
                                  : tr('kbTargetNone');
    }
    // AP connected client count
    if (s.APCLIENTS !== undefined) {
      const el = document.getElementById('apClientCount');
      if (el) el.textContent = s.APCLIENTS;
    }
    // BLE connection status
    if (s.BLECONN !== undefined) {
      const el = document.getElementById('bleConnStatus');
      if (el) el.textContent = s.BLECONN === 'ON' ? tr('connected') : tr('noDevices');
    }
  }

  function setToggle(id, on) {
    const el = document.getElementById(id);
    if (el) el.checked = on;
  }

  function setModesEnabled(enabled) {
    document.querySelectorAll('#modesGrid input[data-mode]').forEach(cb => cb.disabled = !enabled);
    document.getElementById('detail-wifi').classList.toggle('is-off', !enabled);
    document.getElementById('detail-kb').classList.toggle('is-off', !enabled);
    const tp = document.getElementById('detail-touchpad');
    if (tp) tp.classList.toggle('is-off', !enabled);
    const vlt = document.getElementById('detail-vault');
    if (vlt) vlt.classList.toggle('is-off', !enabled);
    if (!enabled) {
      ['detail-jig', 'detail-ap', 'detail-ble'].forEach(id => document.getElementById(id).classList.add('is-off'));
    }
  }
  setModesEnabled(false);

  document.querySelectorAll('#modesGrid input[type=checkbox]').forEach(cb => {
    cb.addEventListener('change', () => {
      const mode = cb.getAttribute('data-mode');
      send('CMD:' + mode + ':' + (cb.checked ? 'ON' : 'OFF'));
    });
  });

  // --- Remote Keyboard ---
  // Chunked rather than one giant CMD:INJECT:STRING: — handleCommand() on
  // the device runs synchronously inside loop(), so a very long paste sent
  // as a single message would stall AP-client polling / jiggler timing /
  // WS housekeeping for however long that whole string takes to type out.
  // Small chunks with a short gap between them keep the device responsive
  // mid-paste without changing what actually lands on the target machine.
  const KB_CHUNK_SIZE = 300;
  const KB_CHUNK_GAP_MS = 60;
  const kbText = document.getElementById('kbText');
  const kbSend = document.getElementById('kbSend');
  const kbClear = document.getElementById('kbClear');

  function sendRemoteText(text) {
    if (!text) return;
    let i = 0;
    (function pump() {
      if (i >= text.length) return;
      const chunk = text.slice(i, i + KB_CHUNK_SIZE);
      i += KB_CHUNK_SIZE;
      send('CMD:INJECT:STRING:' + chunk);
      if (i < text.length) setTimeout(pump, KB_CHUNK_GAP_MS);
    })();
  }

  kbSend.addEventListener('click', () => sendRemoteText(kbText.value));
  kbText.addEventListener('keydown', (e) => {
    if (e.key === 'Enter' && (e.ctrlKey || e.metaKey)) {
      e.preventDefault();
      sendRemoteText(kbText.value);
    }
  });
  kbClear.addEventListener('click', () => { kbText.value = ''; kbText.focus(); });

  document.querySelectorAll('#kbSpecialKeys .slider-point[data-macro]').forEach(btn => {
    btn.addEventListener('click', () => send('CMD:INJECT:MACRO:' + btn.getAttribute('data-macro')));
  });

  document.querySelectorAll('#jigTimePoints .slider-point').forEach(pt => {
    pt.addEventListener('click', () => send('CMD:JIG:TIME:' + pt.getAttribute('data-time')));
  });

  document.querySelectorAll('[data-show-target]').forEach(btn => {
    btn.addEventListener('click', () => {
      const el = document.getElementById(btn.getAttribute('data-show-target'));
      el.style.display = el.style.display === 'none' ? 'block' : 'none';
    });
  });

  const paramsModal = document.getElementById('paramsModal');
  const brightnessSlider = document.getElementById('brightnessSlider');
  const brightVal = document.getElementById('brightVal');
  const timeoutSelect = document.getElementById('timeoutSelect');

  document.getElementById('openParams').addEventListener('click', () => paramsModal.classList.add('show'));
  document.getElementById('closeParams').addEventListener('click', () => paramsModal.classList.remove('show'));

  brightnessSlider.addEventListener('input', () => { brightVal.textContent = brightnessSlider.value; });
  brightnessSlider.addEventListener('change', () => send('CMD:BRIGHT:' + brightnessSlider.value));
  timeoutSelect.addEventListener('change', () => send('CMD:TOUT:' + timeoutSelect.value));
  document.querySelectorAll('#menuModePoints .slider-point').forEach(pt => {
    pt.addEventListener('click', () => send('CMD:MENU:' + pt.getAttribute('data-mode')));
  });

  let isDragging = false, offsetX = 0, offsetY = 0;
  const paramsHeader = document.getElementById('paramsHeader');
  paramsHeader.addEventListener('mousedown', (e) => {
    if (e.target.closest('button')) return;
    isDragging = true;
    const rect = paramsModal.getBoundingClientRect();
    offsetX = e.clientX - rect.left; offsetY = e.clientY - rect.top;
  });
  document.addEventListener('mousemove', (e) => {
    if (!isDragging) return;
    paramsModal.style.transform = 'none';
    paramsModal.style.left = (e.clientX - offsetX) + 'px';
    paramsModal.style.top = (e.clientY - offsetY) + 'px';
  });
  document.addEventListener('mouseup', () => isDragging = false);

  // ----------------------------------------------------------------
  // F-key row
  // ----------------------------------------------------------------
  document.querySelectorAll('[data-fkey]').forEach(btn => {
    btn.addEventListener('click', () => send('CMD:INJECT:MACRO:' + btn.getAttribute('data-fkey')));
  });

  // ----------------------------------------------------------------
  // Touchpad — fixes:
  // 1. setPointerCapture so tracking continues after finger leaves the element
  // 2. Throttled to 16ms (≈60fps) to avoid flooding the WS/serial link
  // 3. Cleaned up on pointerup/pointercancel so a second drag starts fresh
  // ----------------------------------------------------------------
  document.getElementById('touchpadToggle').addEventListener('click', () => {
    const s = document.getElementById('touchpadSection');
    const btn = document.getElementById('touchpadToggle');
    const vis = s.style.display === 'none' || !s.style.display;
    s.style.display = vis ? 'block' : 'none';
    btn.textContent = vis ? '▲' : tr('showHide');
  });

  const touchpadArea = document.getElementById('touchpadArea');
  if (touchpadArea) {
    let tpLastX = 0, tpLastY = 0, tpActive = false, tpLastSend = 0;
    const TP_THROTTLE_MS = 16;
    const TP_SENSITIVITY = 1.5;

    touchpadArea.addEventListener('pointerdown', (e) => {
      touchpadArea.setPointerCapture(e.pointerId); // keeps tracking outside the element
      tpLastX = e.clientX; tpLastY = e.clientY;
      tpActive = true;
      touchpadArea.classList.add('active');
      e.preventDefault();
    });

    touchpadArea.addEventListener('pointermove', (e) => {
      if (!tpActive) return;
      const now = Date.now();
      if (now - tpLastSend < TP_THROTTLE_MS) return;
      tpLastSend = now;
      let dx = Math.round((e.clientX - tpLastX) * TP_SENSITIVITY);
      let dy = Math.round((e.clientY - tpLastY) * TP_SENSITIVITY);
      tpLastX = e.clientX; tpLastY = e.clientY;
      // Clamp to int8 range that the firmware expects
      dx = Math.max(-127, Math.min(127, dx));
      dy = Math.max(-127, Math.min(127, dy));
      if (dx !== 0 || dy !== 0) send('CMD:MOUSE:MOVE:' + dx + ':' + dy);
      e.preventDefault();
    });

    const tpEnd = (e) => {
      tpActive = false;
      touchpadArea.classList.remove('active');
    };
    touchpadArea.addEventListener('pointerup', tpEnd);
    touchpadArea.addEventListener('pointercancel', tpEnd);

    // Left / Right click buttons
    const mlBtn = document.getElementById('mouseLeft');
    const mrBtn = document.getElementById('mouseRight');
    if (mlBtn) mlBtn.addEventListener('click', () => send('CMD:MOUSE:CLICK:LEFT'));
    if (mrBtn) mrBtn.addEventListener('click', () => send('CMD:MOUSE:CLICK:RIGHT'));
  }

  // ----------------------------------------------------------------
  // Password Vault — persisted on the firmware (NVS) and mirrored in
  // localStorage for the UI to render without waiting for a firmware
  // response every time. VAULT:LIST? on connect refreshes from firmware.
  // ----------------------------------------------------------------
  function renderVaultFromFirmware(payload) {
    // payload: "0|Gmail|user@gmail.com;1|Router|admin;..."
    const vaultList = document.getElementById('vaultList');
    if (!vaultList) return;
    vaultList.innerHTML = '';
    const entries = payload.split(';').filter(Boolean);
    entries.forEach(entry => {
      const parts = entry.split('|');
      if (parts.length < 3) return;
      const slotIdx = parseInt(parts[0], 10);
      const name = parts[1];
      const user = parts.slice(2).join('|');
      if (!Number.isInteger(slotIdx) || slotIdx < 0 || slotIdx >= 8 || !name) return;

      const row = document.createElement('div');
      row.className = 'vault-entry';

      const runBtn = document.createElement('button');
      runBtn.type = 'button';
      runBtn.className = 'mini-btn';
      runBtn.title = 'Injecter';
      runBtn.setAttribute('data-vault-run', String(slotIdx));
      runBtn.textContent = '▶';
      runBtn.addEventListener('click', () => {
        runBtn.textContent = '⌨…';
        setTimeout(() => { runBtn.textContent = '▶'; }, 1500);
        send('CMD:VAULT:RUN:' + slotIdx);
      });

      const info = document.createElement('div');
      const nameEl = document.createElement('div');
      nameEl.className = 'vault-name';
      nameEl.textContent = name;
      const userEl = document.createElement('div');
      userEl.className = 'vault-user';
      userEl.textContent = user;
      info.appendChild(nameEl);
      info.appendChild(userEl);

      const delBtn = document.createElement('button');
      delBtn.type = 'button';
      delBtn.className = 'macro-del';
      delBtn.setAttribute('data-vault-del', String(slotIdx));
      delBtn.textContent = '✕';
      delBtn.addEventListener('click', () => {
        send('CMD:VAULT:DEL:' + slotIdx);
        setTimeout(() => send('CMD:VAULT:LIST?'), 300);
      });

      row.appendChild(runBtn);
      row.appendChild(info);
      row.appendChild(delBtn);
      vaultList.appendChild(row);
    });
  }

  const addVaultBtn = document.getElementById('addVaultBtn');
  const vaultForm = document.getElementById('vaultForm');
  const vaultCancel = document.getElementById('vaultCancel');
  const vaultSaveBtn = document.getElementById('vaultSaveBtn');

  if (addVaultBtn) addVaultBtn.addEventListener('click', () => vaultForm.classList.toggle('show'));
  if (vaultCancel) vaultCancel.addEventListener('click', () => vaultForm.classList.remove('show'));
  if (vaultSaveBtn) vaultSaveBtn.addEventListener('click', () => {
    const name = document.getElementById('newVaultName').value.trim();
    const user = document.getElementById('newVaultUser').value.trim();
    const pass = document.getElementById('newVaultPass').value;
    if (!name || !pass) return;
    if (name.includes('|') || name.includes(';') || user.includes('|') || user.includes(';')) {
      showError(tr('vaultBadChars'));
      return;
    }
    const usedSlots = new Set(
      Array.from(document.querySelectorAll('#vaultList [data-vault-run]'))
        .map(el => parseInt(el.getAttribute('data-vault-run'), 10))
        .filter(n => Number.isInteger(n) && n >= 0 && n < 8)
    );
    let slot = -1;
    for (let i = 0; i < 8; i++) {
      if (!usedSlots.has(i)) { slot = i; break; }
    }
    if (slot === -1) {
      showError(tr('vaultFull'));
      return;
    }
    send('CMD:VAULT:SET:' + slot + ':' + name + '|' + user + '|' + pass);
    setTimeout(() => send('CMD:VAULT:LIST?'), 400);
    document.getElementById('newVaultName').value = '';
    document.getElementById('newVaultUser').value = '';
    document.getElementById('newVaultPass').value = '';
    vaultForm.classList.remove('show');
  });

  // ----------------------------------------------------------------
  // Custom Macro persistence — saved in localStorage, rendered on load,
  // sent to firmware on each press. Before this fix, macros were added
  // only to the DOM and lost on every page reload.
  // ----------------------------------------------------------------
  if (!state.customMacros) state.customMacros = [];

  function renderCustomMacros() {
    const list = document.getElementById('customMacroList');
    if (!list) return;
    list.innerHTML = '';
    state.customMacros.forEach((m, i) => {
      const row = document.createElement('div');
      row.className = 'macro-row';
      const btn = document.createElement('button');
      btn.type = 'button';
      btn.className = 'macro-run-btn';
      btn.textContent = m.name;
      btn.addEventListener('click', () => send('CMD:INJECT:MACRO:' + m.keys));
      const detail = document.createElement('span');
      detail.className = 'macro-detail';
      detail.textContent = m.keys;
      const del = document.createElement('button');
      del.type = 'button';
      del.className = 'macro-del';
      del.textContent = '✕';
      del.addEventListener('click', () => {
        state.customMacros.splice(i, 1);
        saveUiState();
        renderCustomMacros();
      });
      row.appendChild(btn);
      row.appendChild(detail);
      row.appendChild(del);
      list.appendChild(row);
    });
  }

  const addMacroBtn = document.getElementById('addMacroBtn');
  const macroForm = document.getElementById('macroForm');
  if (addMacroBtn && macroForm) {
    addMacroBtn.addEventListener('click', () => macroForm.classList.toggle('show'));
    const newMacroName = document.getElementById('newMacroName');
    const newMacroKeys = document.getElementById('newMacroKeys');
    const cancelMacroBtn = document.getElementById('cancelMacroBtn');
    const saveMacroBtn = document.getElementById('saveMacroBtn');
    if (cancelMacroBtn) cancelMacroBtn.addEventListener('click', () => macroForm.classList.remove('show'));
    if (saveMacroBtn) saveMacroBtn.addEventListener('click', () => {
      const name = newMacroName ? newMacroName.value.trim() : '';
      const keys = newMacroKeys ? newMacroKeys.value.trim().toUpperCase() : '';
      if (!name || !keys) return;
      state.customMacros.push({ name, keys });
      saveUiState();
      renderCustomMacros();
      if (newMacroName) newMacroName.value = '';
      if (newMacroKeys) newMacroKeys.value = '';
      macroForm.classList.remove('show');
    });
  }

  renderCustomMacros();

  applyTheme();
  applyLang();
  connectSocket();
  tryAutoReconnect();
})();
</script>
</body>
</html>
)rawliteral";
#endif