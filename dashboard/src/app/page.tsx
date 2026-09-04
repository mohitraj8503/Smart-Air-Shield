'use client';

import React, { useState, useEffect } from 'react';
import { Bluetooth, BluetoothConnected, BluetoothOff, Play, Shield, Info, AlertCircle } from 'lucide-react';
import { bleManager } from '../ble/bleManager';
import { TelemetryData, ConnectionState, HistoryPoint } from '../types';
import { LiveAQI } from '../components/LiveAQI';
import { FanControl } from '../components/FanControl';
import { BatteryIndicator } from '../components/BatteryIndicator';
import { SessionHistory } from '../components/SessionHistory';

export default function DashboardPage() {
  const [telemetry, setTelemetry] = useState<TelemetryData | null>(null);
  const [connectionState, setConnectionState] = useState<ConnectionState>('disconnected');
  const [errorMessage, setErrorMessage] = useState<string | null>(null);
  const [history, setHistory] = useState<HistoryPoint[]>([]);

  useEffect(() => {
    bleManager.setCallbacks(
      (data: TelemetryData) => {
        setTelemetry(data);

        // Append to history point
        const timeStr = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
        setHistory((prev) => [
          ...prev.slice(-60), // Keep last 60 points in memory
          {
            time: timeStr,
            pm25_amb: data.pm25_ambient,
            pm25_out: data.pm25_outlet,
            duty: data.duty,
          },
        ]);
      },
      (state: ConnectionState, err?: string) => {
        setConnectionState(state);
        if (err) {
          setErrorMessage(err);
        } else {
          setErrorMessage(null);
        }
      }
    );

    return () => {
      bleManager.disconnect();
    };
  }, []);

  const handleConnect = async () => {
    setErrorMessage(null);
    await bleManager.connect();
  };

  const handleDisconnect = () => {
    bleManager.disconnect();
  };

  const handleStartMock = () => {
    bleManager.startMockMode();
  };

  return (
    <main className="min-h-screen bg-slate-950 text-slate-100 p-4 md:p-8 max-w-5xl mx-auto space-y-6">
      {/* Top Navbar Header */}
      <header className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 pb-6 border-b border-slate-800/80">
        <div>
          <div className="flex items-center space-x-2.5">
            <div className="p-2.5 rounded-2xl bg-cyan-500/10 border border-cyan-500/20 text-cyan-400 shadow-inner">
              <Shield className="w-6 h-6" />
            </div>
            <div>
              <h1 className="text-xl md:text-2xl font-black tracking-tight text-white flex items-center gap-2">
                SMART AIR-SHIELD
                <span className="text-[10px] uppercase font-bold tracking-widest px-2 py-0.5 rounded-full bg-cyan-500/10 border border-cyan-500/30 text-cyan-400">
                  Vishwakarma 2026–27
                </span>
              </h1>
              <p className="text-xs text-slate-400 font-medium">
                Removable Air-Purification & Monitoring Module for Two-Wheeler Helmets
              </p>
            </div>
          </div>
        </div>

        {/* Connection Control Buttons */}
        <div className="flex items-center space-x-2">
          {connectionState === 'disconnected' && (
            <>
              <button
                onClick={handleConnect}
                className="py-2 px-4 bg-cyan-500 hover:bg-cyan-400 text-slate-950 font-bold text-xs rounded-xl shadow-lg shadow-cyan-500/20 transition-all flex items-center space-x-2"
              >
                <Bluetooth className="w-4 h-4" />
                <span>Pair Helmet</span>
              </button>
              <button
                onClick={handleStartMock}
                className="py-2 px-3 bg-slate-800 hover:bg-slate-700 text-slate-300 font-semibold text-xs rounded-xl border border-slate-700 transition-all flex items-center space-x-1.5"
                title="Simulate live helmet telemetry without hardware"
              >
                <Play className="w-3.5 h-3.5 text-emerald-400" />
                <span>Demo Mode</span>
              </button>
            </>
          )}

          {connectionState === 'connecting' && (
            <button
              disabled
              className="py-2 px-4 bg-slate-800 text-slate-400 font-semibold text-xs rounded-xl border border-slate-700 flex items-center space-x-2 cursor-wait"
            >
              <Bluetooth className="w-4 h-4 animate-pulse text-cyan-400" />
              <span>Scanning...</span>
            </button>
          )}

          {connectionState === 'connected' && (
            <button
              onClick={handleDisconnect}
              className="py-2 px-4 bg-emerald-500/10 hover:bg-rose-500/10 border border-emerald-500/30 hover:border-rose-500/30 text-emerald-400 hover:text-rose-400 font-bold text-xs rounded-xl transition-all flex items-center space-x-2"
            >
              <BluetoothConnected className="w-4 h-4" />
              <span>Connected (Disconnect)</span>
            </button>
          )}

          {connectionState === 'mock' && (
            <button
              onClick={handleDisconnect}
              className="py-2 px-4 bg-purple-500/10 hover:bg-rose-500/10 border border-purple-500/30 hover:border-rose-500/30 text-purple-400 hover:text-rose-400 font-bold text-xs rounded-xl transition-all flex items-center space-x-2"
            >
              <Play className="w-4 h-4" />
              <span>Demo Mode Active (Exit)</span>
            </button>
          )}
        </div>
      </header>

      {/* Error / Browser Notice Banner */}
      {errorMessage && (
        <div className="bg-rose-500/10 border border-rose-500/30 rounded-2xl p-4 flex items-start space-x-3 text-rose-300 text-xs">
          <AlertCircle className="w-5 h-5 text-rose-400 shrink-0 mt-0.5" />
          <div>
            <div className="font-bold">Bluetooth Notice:</div>
            <div>{errorMessage}</div>
          </div>
        </div>
      )}

      {/* Browser Compatibility Info Tip */}
      <div className="bg-slate-900/50 border border-slate-800/80 rounded-2xl p-3.5 flex items-center space-x-3 text-xs text-slate-400">
        <Info className="w-4 h-4 text-cyan-400 shrink-0" />
        <div>
          <span className="font-semibold text-slate-300">Web Bluetooth Compatibility:</span> Runs natively on Chrome & Edge (Desktop & Android). iOS Safari does not support Web Bluetooth — use Demo Mode or Bluefy on iOS.
        </div>
      </div>

      {/* Main Grid Components */}
      <div className="space-y-6">
        {/* Live AQI & Dual Sensor Cards */}
        <LiveAQI telemetry={telemetry} />

        {/* Blower & Battery Grid */}
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          <FanControl telemetry={telemetry} />
          <BatteryIndicator telemetry={telemetry} />
        </div>

        {/* Live Exposure Trend History & CSV Export */}
        <SessionHistory history={history} />
      </div>

      {/* Project Footer */}
      <footer className="pt-8 pb-4 text-center text-xs text-slate-500 border-t border-slate-800/60 space-y-2">
        <p>SMART AIR-SHIELD &bull; Submitted for Vishwakarma Awards 2026–27 (IIT Hyderabad Track)</p>
        <p className="text-[11px] text-slate-600">
          Sustainable Cities &bull; Clean Water, Sanitation & Air Quality Monitoring &bull; Non-structural helmet attachment
        </p>
      </footer>
    </main>
  );
}
