'use client';

import React, { useState, useEffect } from 'react';
import { Bluetooth, Play, Info } from 'lucide-react';
import { bleManager } from '../ble/bleManager';
import { TelemetryData, ConnectionState, HistoryPoint } from '../types';
import { LiveAQI } from '../components/LiveAQI';
import { FanControl } from '../components/FanControl';
import { BatteryIndicator } from '../components/BatteryIndicator';
import { SessionHistory } from '../components/SessionHistory';

export default function DashboardPage() {
  const [telemetry, setTelemetry] = useState<TelemetryData | null>(null);
  const [connectionState, setConnectionState] = useState<ConnectionState>('disconnected');
  const [statusMessage, setStatusMessage] = useState<string | null>(null);
  const [history, setHistory] = useState<HistoryPoint[]>([]);

  useEffect(() => {
    bleManager.setCallbacks(
      (data: TelemetryData) => {
        setTelemetry(data);

        const timeStr = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
        setHistory((prev) => [
          ...prev.slice(-60),
          {
            time: timeStr,
            pm25_amb: data.pm25_ambient,
            pm25_out: data.pm25_outlet,
            duty: data.duty,
          },
        ]);
      },
      (state: ConnectionState, msg?: string) => {
        setConnectionState(state);
        if (msg) {
          setStatusMessage(msg);
        } else {
          setStatusMessage(null);
        }
      }
    );

    return () => {
      bleManager.disconnect();
    };
  }, []);

  const handleConnect = async () => {
    setStatusMessage(null);
    await bleManager.connect();
  };

  const handleDisconnect = () => {
    bleManager.disconnect();
  };

  const handleStartMock = () => {
    bleManager.startMockMode();
  };

  return (
    <main className="min-h-screen bg-[#F5F5F7] text-[#1D1D1F] p-4 sm:p-6 md:p-10 max-w-4xl mx-auto space-y-6">
      {/* Apple-style Clean Navigation Bar */}
      <header className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 pb-4 border-b border-black/[0.06]">
        <div className="text-left">
          <div className="flex items-center space-x-2">
            <h1 className="text-xl font-bold tracking-tight text-[#1D1D1F]">
              SMART AIR-SHIELD
            </h1>
            <span className="text-[11px] font-medium px-2 py-0.5 rounded-full bg-black/[0.04] text-[#6E6E73]">
              Vishwakarma 26–27
            </span>
          </div>
          <p className="text-xs text-[#6E6E73] mt-0.5">
            Removable air-purification & monitoring module for two-wheeler helmets
          </p>
        </div>

        {/* Pairing & Action Buttons */}
        <div className="flex items-center space-x-2.5">
          {connectionState === 'disconnected' && (
            <>
              <button
                onClick={handleConnect}
                className="py-2 px-4 bg-[#0A84FF] hover:bg-[#0071E3] text-white font-medium text-xs rounded-full shadow-sm transition-all flex items-center space-x-1.5 active:scale-95"
              >
                <Bluetooth className="w-3.5 h-3.5" />
                <span>Pair helmet</span>
              </button>
              <button
                onClick={handleStartMock}
                className="py-2 px-3.5 bg-white hover:bg-[#E5E5EA]/40 text-[#1D1D1F] font-medium text-xs rounded-full border border-black/[0.08] shadow-sm transition-all flex items-center space-x-1.5 active:scale-95"
                title="Preview live helmet telemetry without hardware"
              >
                <Play className="w-3 h-3 text-[#34C759]" />
                <span>Demo mode</span>
              </button>
            </>
          )}

          {connectionState === 'connecting' && (
            <div className="py-2 px-4 bg-white text-[#6E6E73] font-medium text-xs rounded-full border border-black/[0.08] shadow-sm flex items-center space-x-2">
              <span className="w-2 h-2 rounded-full bg-[#0A84FF] animate-ping" />
              <span>Connecting...</span>
            </div>
          )}

          {connectionState === 'connected' && (
            <button
              onClick={handleDisconnect}
              className="py-2 px-4 bg-white hover:bg-black/[0.03] text-[#34C759] font-medium text-xs rounded-full border border-black/[0.08] shadow-sm transition-all flex items-center space-x-2"
            >
              <span className="w-2 h-2 rounded-full bg-[#34C759]" />
              <span>Connected &bull; Disconnect</span>
            </button>
          )}

          {connectionState === 'mock' && (
            <button
              onClick={handleDisconnect}
              className="py-2 px-4 bg-white hover:bg-black/[0.03] text-[#0A84FF] font-medium text-xs rounded-full border border-black/[0.08] shadow-sm transition-all flex items-center space-x-2"
            >
              <span className="w-2 h-2 rounded-full bg-[#0A84FF]" />
              <span>Demo active &bull; Exit</span>
            </button>
          )}
        </div>
      </header>

      {/* Calm Status Notice */}
      {statusMessage && (
        <div className="bg-white rounded-2xl p-4 border border-black/[0.06] shadow-sm flex items-start space-x-3 text-[#6E6E73] text-xs text-left">
          <Info className="w-4 h-4 text-[#0A84FF] shrink-0 mt-0.5" />
          <div>{statusMessage}</div>
        </div>
      )}

      {/* Main Apple-Inspired Dashboard Layout */}
      <div className="space-y-6">
        {/* 1. The Hero: Breathing Zone Particulate Reduction */}
        <LiveAQI telemetry={telemetry} />

        {/* 2. Quiet, Disciplined Controls & Battery Grid */}
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          <FanControl telemetry={telemetry} />
          <BatteryIndicator telemetry={telemetry} />
        </div>

        {/* 3. Commute Exposure Trend History */}
        <SessionHistory history={history} />
      </div>

      {/* Quiet Footer */}
      <footer className="pt-6 pb-2 text-center text-xs text-[#6E6E73] space-y-1">
        <p>SMART AIR-SHIELD &bull; Vishwakarma Awards 2026–27 &bull; IIT Hyderabad Track</p>
        <p className="text-[11px] text-[#6E6E73]/70">
          Sustainable Cities &bull; Clean Water, Sanitation & Air Quality Monitoring &bull; Non-structural accessory
        </p>
      </footer>
    </main>
  );
}
