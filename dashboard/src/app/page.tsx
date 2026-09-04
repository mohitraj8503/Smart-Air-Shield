'use client';

import React, { useState, useEffect } from 'react';
import { Bluetooth, Radio, Shield, CheckCircle2 } from 'lucide-react';
import { bleManager } from '../ble/bleManager';
import { TelemetryData, ConnectionState, HistoryPoint } from '../types';
import { LiveAQI } from '../components/LiveAQI';
import { FanControl } from '../components/FanControl';
import { BatteryIndicator } from '../components/BatteryIndicator';
import { SessionHistory } from '../components/SessionHistory';

export default function DashboardPage() {
  const [telemetry, setTelemetry] = useState<TelemetryData | null>(null);
  const [connectionState, setConnectionState] = useState<ConnectionState>('mock');
  const [statusMessage, setStatusMessage] = useState<string | null>(null);
  const [history, setHistory] = useState<HistoryPoint[]>([]);

  useEffect(() => {
    // Automatically start live simulated feed on initial load so dashboard is never empty
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

    bleManager.startMockMode();

    return () => {
      bleManager.disconnect();
    };
  }, []);

  const handleConnect = async () => {
    setStatusMessage(null);
    await bleManager.connect();
  };

  const handleUseDemo = () => {
    setStatusMessage(null);
    bleManager.startMockMode();
  };

  return (
    <main className="min-h-screen bg-[#F5F5F7] text-[#1D1D1F] max-w-[1280px] mx-auto px-6 sm:px-10 py-8 space-y-6">
      {/* Apple-Style Navigation Header */}
      <header className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 pb-5 border-b border-black/[0.06]">
        <div className="text-left">
          <div className="flex items-center space-x-3">
            <div className="w-9 h-9 rounded-2xl bg-[#0A84FF]/10 border border-[#0A84FF]/20 flex items-center justify-center text-[#0A84FF]">
              <Shield className="w-5 h-5" />
            </div>
            <div>
              <div className="flex items-center space-x-2">
                <h1 className="text-2xl sm:text-3xl font-bold tracking-tight text-[#1D1D1F]">
                  SMART AIR-SHIELD
                </h1>
                <span className="text-[11px] font-semibold px-2.5 py-0.5 rounded-full bg-black/[0.04] text-[#6E6E73]">
                  Vishwakarma 26–27
                </span>
              </div>
              <p className="text-xs sm:text-sm text-[#6E6E73] mt-0.5 font-normal">
                Removable air-purification & monitoring module for two-wheeler helmets
              </p>
            </div>
          </div>
        </div>

        {/* Pairing & Live Feed Status Controls */}
        <div className="flex items-center space-x-2.5">
          {connectionState === 'connected' ? (
            <div className="flex items-center space-x-2 px-4 py-2 bg-white rounded-full border border-black/[0.08] shadow-sm text-xs font-semibold text-[#34C759]">
              <span className="w-2 h-2 rounded-full bg-[#34C759] animate-pulse" />
              <span>Connected to ESP32 Hardware</span>
            </div>
          ) : (
            <div className="flex items-center space-x-2 bg-white p-1 rounded-full border border-black/[0.08] shadow-sm">
              <button
                onClick={handleUseDemo}
                className={`py-1.5 px-4 text-xs font-semibold rounded-full transition-all flex items-center space-x-1.5 ${
                  connectionState === 'mock'
                    ? 'bg-[#F5F5F7] text-[#1D1D1F] shadow-sm'
                    : 'text-[#6E6E73] hover:text-[#1D1D1F]'
                }`}
              >
                <Radio className="w-3.5 h-3.5 text-[#34C759]" />
                <span>Live Feed</span>
              </button>

              <button
                onClick={handleConnect}
                className="py-1.5 px-4 bg-[#0A84FF] hover:bg-[#0071E3] text-white text-xs font-semibold rounded-full transition-all flex items-center space-x-1.5 shadow-sm active:scale-95"
                title="Connect physical ESP32 device via Bluetooth"
              >
                <Bluetooth className="w-3.5 h-3.5" />
                <span>Pair Bluetooth</span>
              </button>
            </div>
          )}
        </div>
      </header>

      {/* Subtle Apple Informational Toast (if needed) */}
      {statusMessage && (
        <div className="bg-white rounded-2xl p-4 border border-black/[0.06] shadow-sm flex items-center justify-between text-xs text-[#6E6E73] text-left">
          <div className="flex items-center space-x-2.5">
            <CheckCircle2 className="w-4 h-4 text-[#34C759] shrink-0" />
            <span>{statusMessage}</span>
          </div>
          <button
            onClick={() => setStatusMessage(null)}
            className="text-[#6E6E73] hover:text-[#1D1D1F] font-medium ml-4 text-[11px]"
          >
            Dismiss
          </button>
        </div>
      )}

      {/* Expansive Dashboard Sections */}
      <div className="space-y-6">
        {/* 1. Hero: Zoomed-in Breathing Zone Particulate Reduction */}
        <LiveAQI telemetry={telemetry} />

        {/* 2. Controls & Battery Grid */}
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          <FanControl telemetry={telemetry} />
          <BatteryIndicator telemetry={telemetry} />
        </div>

        {/* 3. Commute Exposure Trend History */}
        <SessionHistory history={history} />
      </div>

      {/* Clean Apple Footer */}
      <footer className="pt-8 pb-4 text-center text-xs text-[#6E6E73] space-y-1 border-t border-black/[0.04]">
        <p>SMART AIR-SHIELD &bull; Vishwakarma Awards 2026–27 &bull; IIT Hyderabad Track</p>
        <p className="text-[11px] text-[#6E6E73]/70">
          Sustainable Cities &bull; Clean Water, Sanitation & Air Quality Monitoring &bull; Non-structural helmet accessory
        </p>
      </footer>
    </main>
  );
}
