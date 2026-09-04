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
    <div className="bg-white rounded-3xl p-6 sm:p-8 border border-black/[0.06] shadow-[0_2px_16px_rgba(0,0,0,0.03)] space-y-6 text-left">
      <div className="flex items-center justify-between">
        <div>
          <div className="text-sm font-medium text-[#6E6E73]">
            Battery level
          </div>
          <h3 className="text-xl font-bold text-[#1D1D1F] tracking-tight mt-0.5">
            Li-ion power pack
          </h3>
        </div>

        <div className="flex items-center space-x-2.5">
          <span className="text-3xl sm:text-4xl font-extrabold tracking-tight text-[#1D1D1F]">
            {batteryPct}<span className="text-base font-medium text-[#6E6E73] ml-0.5">%</span>
          </span>
          <div className="w-10 h-10 rounded-full bg-[#F5F5F7] flex items-center justify-center text-[#1D1D1F]">
            <Battery className="w-5 h-5 text-[#1D1D1F]" />
          </div>
        </div>
      </div>

      {/* iOS Battery Capsule (Thicker & More Prominent) */}
      <div className="w-full bg-[#E5E5EA] h-3.5 rounded-full overflow-hidden p-0.5">
        <div
          className={`h-full rounded-full transition-all duration-500 ${
            isLow ? 'bg-[#FF3B30]' : 'bg-[#34C759]'
          }`}
          style={{ width: `${Math.min(100, Math.max(4, batteryPct))}%` }}
        />
      </div>

      {/* Confident Metrics: Runtime & Voltage (Zoomed in) */}
      <div className="grid grid-cols-2 gap-6 pt-1">
        <div className="space-y-1">
          <div className="text-sm font-medium text-[#6E6E73]">Estimated runtime</div>
          <div className="text-2xl sm:text-3xl font-extrabold tracking-tight text-[#1D1D1F]">
            ~{estHours} <span className="text-sm font-normal text-[#6E6E73]">hours</span>
          </div>
        </div>

        <div className="space-y-1">
          <div className="text-sm font-medium text-[#6E6E73]">Pack voltage</div>
          <div className="text-2xl sm:text-3xl font-extrabold tracking-tight text-[#1D1D1F]">
            {approxVoltage} <span className="text-sm font-normal text-[#6E6E73]">V</span>
          </div>
        </div>
      </div>
    </div>
  );
};
