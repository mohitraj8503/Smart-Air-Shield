'use client';

import React from 'react';
import { Battery, BatteryCharging, BatteryWarning, Clock } from 'lucide-react';
import { TelemetryData } from '../types';

interface BatteryIndicatorProps {
  telemetry: TelemetryData | null;
}

export const BatteryIndicator: React.FC<BatteryIndicatorProps> = ({ telemetry }) => {
  const batteryPct = telemetry ? telemetry.battery : 85;
  const isLow = batteryPct < 10;

  // 2S Li-ion approximate voltage calculation (6.0V to 8.4V)
  const approxVoltage = (6.0 + (batteryPct / 100) * 2.4).toFixed(2);

  // Runtime estimate (6-8 hours full nominal runtime)
  const estHours = ((batteryPct / 100) * 7.5).toFixed(1);

  return (
    <div className="bg-slate-900/80 border border-slate-800/90 rounded-2xl p-5 shadow-lg space-y-3">
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-2">
          <div className={`p-2 rounded-xl ${isLow ? 'bg-rose-500/10 text-rose-400' : 'bg-emerald-500/10 text-emerald-400'}`}>
            {isLow ? <BatteryWarning className="w-5 h-5 animate-pulse" /> : <Battery className="w-5 h-5" />}
          </div>
          <div>
            <h3 className="font-bold text-white text-sm">7.4V Li-ion Power Pack</h3>
            <p className="text-xs text-slate-400">2S 2500mAh BMS Protected</p>
          </div>
        </div>

        <span className={`text-2xl font-black ${isLow ? 'text-rose-400' : 'text-emerald-400'}`}>
          {batteryPct}%
        </span>
      </div>

      {/* Visual Battery Bar */}
      <div className="w-full bg-slate-950 h-3 rounded-full overflow-hidden p-0.5 border border-slate-800">
        <div
          className={`h-full rounded-full transition-all duration-500 ${
            isLow
              ? 'bg-rose-500'
              : batteryPct < 30
              ? 'bg-amber-400'
              : 'bg-emerald-400'
          }`}
          style={{ width: `${Math.min(100, Math.max(2, batteryPct))}%` }}
        />
      </div>

      <div className="grid grid-cols-2 gap-2 pt-1 text-xs">
        <div className="bg-slate-950/50 border border-slate-800/60 rounded-xl p-2.5 flex items-center space-x-2">
          <Clock className="w-4 h-4 text-cyan-400" />
          <div>
            <div className="text-[10px] text-slate-400 font-medium">Est. Runtime</div>
            <div className="font-bold text-slate-200">~{estHours} hrs</div>
          </div>
        </div>

        <div className="bg-slate-950/50 border border-slate-800/60 rounded-xl p-2.5 flex items-center space-x-2">
          <BatteryCharging className="w-4 h-4 text-amber-400" />
          <div>
            <div className="text-[10px] text-slate-400 font-medium">Pack Voltage</div>
            <div className="font-bold text-slate-200">~{approxVoltage} V</div>
          </div>
        </div>
      </div>
    </div>
  );
};
