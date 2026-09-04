'use client';

import React from 'react';
import { Fan } from 'lucide-react';
import { TelemetryData } from '../types';
import { bleManager } from '../ble/bleManager';

interface FanControlProps {
  telemetry: TelemetryData | null;
}

export const FanControl: React.FC<FanControlProps> = ({ telemetry }) => {
  const mode = telemetry ? telemetry.mode : 1;
  const currentDuty = telemetry ? telemetry.duty : 0;

  const handleModeChange = (newMode: number) => {
    bleManager.setMode(newMode);
  };

  const handleDutyChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const val = parseInt(e.target.value, 10);
    bleManager.setDuty(val);
  };

  return (
    <div className="bg-white rounded-3xl p-6 sm:p-7 border border-black/[0.06] shadow-[0_2px_14px_rgba(0,0,0,0.03)] space-y-5 text-left">
      <div className="flex items-center justify-between">
        <div>
          <div className="text-xs font-medium text-[#6E6E73]">
            Blower regulation
          </div>
          <h3 className="text-lg font-semibold text-[#1D1D1F] tracking-tight mt-0.5">
            Centrifugal fan
          </h3>
        </div>

        <div className="flex items-center space-x-2">
          <span className="text-2xl font-bold tracking-tight text-[#1D1D1F]">
            {mode === 0 ? '0' : currentDuty}<span className="text-sm font-normal text-[#6E6E73] ml-0.5">%</span>
          </span>
          <div className="w-8 h-8 rounded-full bg-[#F5F5F7] flex items-center justify-center text-[#1D1D1F]">
            <Fan className={`w-4 h-4 text-[#1D1D1F] ${mode !== 0 ? 'animate-spin' : ''}`} style={{ animationDuration: `${Math.max(0.6, 2.5 - (currentDuty / 100) * 1.8)}s` }} />
          </div>
        </div>
      </div>

      {/* iOS Segmented Control */}
      <div className="bg-[#E5E5EA] p-1 rounded-2xl flex relative">
        <button
          onClick={() => handleModeChange(1)}
          className={`flex-1 py-1.5 px-3 text-xs font-semibold rounded-xl transition-all duration-200 ${
            mode === 1
              ? 'bg-white text-[#1D1D1F] shadow-[0_1px_3px_rgba(0,0,0,0.12),0_1px_2px_rgba(0,0,0,0.08)]'
              : 'text-[#6E6E73] hover:text-[#1D1D1F]'
          }`}
        >
          Auto adaptive
        </button>

        <button
          onClick={() => handleModeChange(2)}
          className={`flex-1 py-1.5 px-3 text-xs font-semibold rounded-xl transition-all duration-200 ${
            mode === 2
              ? 'bg-white text-[#1D1D1F] shadow-[0_1px_3px_rgba(0,0,0,0.12),0_1px_2px_rgba(0,0,0,0.08)]'
              : 'text-[#6E6E73] hover:text-[#1D1D1F]'
          }`}
        >
          Manual
        </button>

        <button
          onClick={() => handleModeChange(0)}
          className={`flex-1 py-1.5 px-3 text-xs font-semibold rounded-xl transition-all duration-200 ${
            mode === 0
              ? 'bg-white text-[#1D1D1F] shadow-[0_1px_3px_rgba(0,0,0,0.12),0_1px_2px_rgba(0,0,0,0.08)]'
              : 'text-[#6E6E73] hover:text-[#1D1D1F]'
          }`}
        >
          Off
        </button>
      </div>

      {/* Speed Slider */}
      <div className="space-y-2 pt-1">
        <div className="flex items-center justify-between text-xs text-[#6E6E73]">
          <span>Airflow velocity</span>
          <span>{mode === 1 ? 'Adaptive (2–8 L/min)' : mode === 0 ? 'Standby' : `${currentDuty}% speed`}</span>
        </div>

        <input
          type="range"
          min="20"
          max="100"
          step="5"
          value={currentDuty || 20}
          onChange={handleDutyChange}
          disabled={mode === 0}
          className="w-full h-1.5 bg-[#E5E5EA] rounded-full appearance-none cursor-pointer accent-[#0A84FF] disabled:opacity-30 transition-all"
        />

        <div className="flex justify-between text-[11px] text-[#6E6E73]/70 font-medium px-0.5">
          <span>Min (20%)</span>
          <span>Acoustic ceiling (&le;80%)</span>
          <span>Max (100%)</span>
        </div>
      </div>
    </div>
  );
};
