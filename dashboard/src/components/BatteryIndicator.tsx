'use client';

import React from 'react';
import { Battery } from 'lucide-react';
import { TelemetryData } from '../types';

interface BatteryIndicatorProps {
  telemetry: TelemetryData | null;
}

export const BatteryIndicator: React.FC<BatteryIndicatorProps> = ({ telemetry }) => {
  const batteryPct = telemetry ? telemetry.battery : 82;
  const isLow = batteryPct < 10;

  // 2S Li-ion pack voltage approximation
  const approxVoltage = (6.0 + (batteryPct / 100) * 2.4).toFixed(1);
  const estHours = ((batteryPct / 100) * 7.5).toFixed(1);

  return (
    <div className="bg-white rounded-3xl p-6 sm:p-7 border border-black/[0.06] shadow-[0_2px_14px_rgba(0,0,0,0.03)] space-y-5 text-left">
      <div className="flex items-center justify-between">
        <div>
          <div className="text-xs font-medium text-[#6E6E73]">
            Battery level
          </div>
          <h3 className="text-lg font-semibold text-[#1D1D1F] tracking-tight mt-0.5">
            Li-ion power pack
          </h3>
        </div>

        <div className="flex items-center space-x-2">
          <span className="text-2xl font-bold tracking-tight text-[#1D1D1F]">
            {batteryPct}<span className="text-sm font-normal text-[#6E6E73] ml-0.5">%</span>
          </span>
          <div className="w-8 h-8 rounded-full bg-[#F5F5F7] flex items-center justify-center text-[#1D1D1F]">
            <Battery className="w-4 h-4 text-[#1D1D1F]" />
          </div>
        </div>
      </div>

      {/* iOS Battery Capsule */}
      <div className="w-full bg-[#E5E5EA] h-2.5 rounded-full overflow-hidden p-0.5">
        <div
          className={`h-full rounded-full transition-all duration-500 ${
            isLow ? 'bg-[#FF3B30]' : 'bg-[#34C759]'
          }`}
          style={{ width: `${Math.min(100, Math.max(4, batteryPct))}%` }}
        />
      </div>

      {/* Confident Metrics: Runtime & Voltage */}
      <div className="grid grid-cols-2 gap-4 pt-1">
        <div className="space-y-0.5">
          <div className="text-xs font-medium text-[#6E6E73]">Estimated runtime</div>
          <div className="text-xl font-bold tracking-tight text-[#1D1D1F]">
            ~{estHours} <span className="text-xs font-normal text-[#6E6E73]">hours</span>
          </div>
        </div>

        <div className="space-y-0.5">
          <div className="text-xs font-medium text-[#6E6E73]">Pack voltage</div>
          <div className="text-xl font-bold tracking-tight text-[#1D1D1F]">
            {approxVoltage} <span className="text-xs font-normal text-[#6E6E73]">V</span>
          </div>
        </div>
      </div>
    </div>
  );
};
