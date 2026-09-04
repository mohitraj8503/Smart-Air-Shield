'use client';

import React from 'react';
import { Fan, Sliders, Power, Sparkles } from 'lucide-react';
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

  const handlePreset = (val: number) => {
    bleManager.setDuty(val);
  };

  const handlePower = () => {
    bleManager.togglePower();
  };

  return (
    <div className="bg-slate-900/80 border border-slate-800/90 rounded-2xl p-5 shadow-lg space-y-4">
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-2">
          <div className="p-2 rounded-xl bg-cyan-500/10 text-cyan-400">
            <Fan className={`w-5 h-5 ${mode !== 0 ? 'animate-spin' : ''}`} style={{ animationDuration: `${Math.max(0.4, 2.5 - (currentDuty / 100) * 2)}s` }} />
          </div>
          <div>
            <h3 className="font-bold text-white text-sm">Centrifugal Blower Control</h3>
            <p className="text-xs text-slate-400">Adaptive PWM breathing-zone ventilation</p>
          </div>
        </div>

        {/* Power Button */}
        <button
          onClick={handlePower}
          className={`p-2.5 rounded-xl border transition-all ${
            mode !== 0
              ? 'bg-emerald-500/10 border-emerald-500/30 text-emerald-400 hover:bg-emerald-500/20'
              : 'bg-slate-800 border-slate-700 text-slate-400 hover:text-white'
          }`}
          title={mode !== 0 ? 'Power Off' : 'Power On'}
        >
          <Power className="w-4 h-4" />
        </button>
      </div>

      {/* Mode Switcher Tabs */}
      <div className="grid grid-cols-3 gap-2 bg-slate-950 p-1 rounded-xl border border-slate-800">
        <button
          onClick={() => handleModeChange(1)}
          className={`py-2 px-3 text-xs font-bold rounded-lg transition-all flex items-center justify-center space-x-1.5 ${
            mode === 1
              ? 'bg-cyan-500 text-slate-950 shadow-md'
              : 'text-slate-400 hover:text-white'
          }`}
        >
          <Sparkles className="w-3.5 h-3.5" />
          <span>Auto Adaptive</span>
        </button>

        <button
          onClick={() => handleModeChange(2)}
          className={`py-2 px-3 text-xs font-bold rounded-lg transition-all flex items-center justify-center space-x-1.5 ${
            mode === 2
              ? 'bg-cyan-500 text-slate-950 shadow-md'
              : 'text-slate-400 hover:text-white'
          }`}
        >
          <Sliders className="w-3.5 h-3.5" />
          <span>Manual</span>
        </button>

        <button
          onClick={() => handleModeChange(0)}
          className={`py-2 px-3 text-xs font-bold rounded-lg transition-all flex items-center justify-center space-x-1.5 ${
            mode === 0
              ? 'bg-rose-500 text-white shadow-md'
              : 'text-slate-400 hover:text-white'
          }`}
        >
          <Power className="w-3.5 h-3.5" />
          <span>Off</span>
        </button>
      </div>

      {/* Speed Readout & Slider */}
      <div className="space-y-3 pt-1">
        <div className="flex items-center justify-between text-xs">
          <span className="text-slate-400">PWM Output Level</span>
          <span className="font-mono font-bold text-cyan-400 text-sm">{currentDuty}%</span>
        </div>

        <input
          type="range"
          min="20"
          max="100"
          step="5"
          value={currentDuty || 20}
          onChange={handleDutyChange}
          disabled={mode === 0}
          className="w-full h-2 bg-slate-800 rounded-lg appearance-none cursor-pointer accent-cyan-400 disabled:opacity-40"
        />

        {/* Preset Buttons */}
        <div className="flex items-center justify-between gap-2 pt-1">
          {[
            { label: 'Low (30%)', val: 30 },
            { label: 'Med (60%)', val: 60 },
            { label: 'High (90%)', val: 90 },
          ].map((preset) => (
            <button
              key={preset.val}
              onClick={() => handlePreset(preset.val)}
              disabled={mode === 0}
              className={`flex-1 py-1.5 px-2 text-xs font-medium rounded-lg border transition-all ${
                currentDuty === preset.val && mode === 2
                  ? 'bg-cyan-500/20 border-cyan-400 text-cyan-300'
                  : 'bg-slate-950/50 border-slate-800 text-slate-400 hover:bg-slate-800 disabled:opacity-40'
              }`}
            >
              {preset.label}
            </button>
          ))}
        </div>
      </div>
    </div>
  );
};
