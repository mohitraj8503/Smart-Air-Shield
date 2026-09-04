'use client';

import React from 'react';
import { ArrowRight, ShieldCheck, CheckCircle2 } from 'lucide-react';
import { TelemetryData } from '../types';

interface LiveAQIProps {
  telemetry: TelemetryData | null;
}

export const LiveAQI: React.FC<LiveAQIProps> = ({ telemetry }) => {
  if (!telemetry) {
    return (
      <div className="bg-white rounded-3xl p-8 border border-black/[0.06] shadow-[0_2px_16px_rgba(0,0,0,0.04)] text-left">
        <div className="flex items-center space-x-3 mb-2">
          <div className="w-2.5 h-2.5 rounded-full bg-[#6E6E73]/40" />
          <span className="text-xs font-medium text-[#6E6E73] tracking-normal">Awaiting connection</span>
        </div>
        <h2 className="text-2xl font-semibold text-[#1D1D1F] tracking-tight">Breathing zone protection</h2>
        <p className="text-sm text-[#6E6E73] mt-1 max-w-lg leading-relaxed">
          Connect your SMART AIR-SHIELD or start demo mode to see real-time ambient roadside air compared with filtered breathing zone delivery.
        </p>
      </div>
    );
  }

  // Real EPA category styling
  const getEpaStyle = (aqi: number) => {
    if (aqi <= 50) {
      return { label: 'Good', color: '#34C759', bg: 'bg-[#34C759]/10 text-[#34C759]' };
    }
    if (aqi <= 100) {
      return { label: 'Moderate', color: '#FFCC00', bg: 'bg-[#FFCC00]/15 text-[#B28900]' };
    }
    if (aqi <= 150) {
      return { label: 'Unhealthy for sensitive groups', color: '#FF9500', bg: 'bg-[#FF9500]/15 text-[#C96B00]' };
    }
    if (aqi <= 200) {
      return { label: 'Unhealthy', color: '#FF3B30', bg: 'bg-[#FF3B30]/10 text-[#FF3B30]' };
    }
    if (aqi <= 300) {
      return { label: 'Very unhealthy', color: '#AF52DE', bg: 'bg-[#AF52DE]/10 text-[#AF52DE]' };
    }
    return { label: 'Hazardous', color: '#8E2A2A', bg: 'bg-[#8E2A2A]/10 text-[#8E2A2A]' };
  };

  const ambEpa = getEpaStyle(telemetry.aqi);
  const reduction = Math.max(0, telemetry.pm25_ambient - telemetry.pm25_outlet);

  return (
    <div className="space-y-4 text-left">
      {/* Hero Card: The One Moment of Boldness */}
      <div className="bg-white rounded-3xl p-6 sm:p-8 border border-black/[0.06] shadow-[0_2px_20px_rgba(0,0,0,0.04)] relative overflow-hidden">
        {/* Subtle status indicator */}
        <div className="flex flex-wrap items-center justify-between gap-3 mb-6 pb-5 border-b border-black/[0.05]">
          <div className="flex items-center space-x-2.5">
            <span className="relative flex h-2.5 w-2.5">
              <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-[#34C759] opacity-75"></span>
              <span className="relative inline-flex rounded-full h-2.5 w-2.5 bg-[#34C759]"></span>
            </span>
            <span className="text-xs font-semibold text-[#1D1D1F] tracking-normal">
              Active filtration
            </span>
            <span className="text-xs text-[#6E6E73]">&bull; 3-stage HEPA H13</span>
          </div>

          {/* Efficiency pill */}
          <div className="inline-flex items-center space-x-1.5 px-3 py-1 rounded-full bg-[#34C759]/10 text-[#34C759] text-xs font-semibold">
            <ShieldCheck className="w-3.5 h-3.5" />
            <span>{telemetry.filtrationEfficiency}% particulate removal</span>
          </div>
        </div>

        {/* The Hero Comparison Readout */}
        <div className="grid grid-cols-1 lg:grid-cols-12 gap-6 items-center">
          {/* Ambient External Air */}
          <div className="lg:col-span-5 space-y-2">
            <div className="text-xs font-medium text-[#6E6E73]">
              Outside ambient air
            </div>
            <div className="flex items-baseline space-x-2">
              <span className="text-6xl sm:text-7xl font-bold tracking-tight text-[#1D1D1F]">
                {telemetry.pm25_ambient}
              </span>
              <span className="text-sm font-medium text-[#6E6E73]">
                &mu;g/m&sup3; <span className="text-xs text-[#6E6E73]/70 font-normal">PM2.5</span>
              </span>
            </div>

            <div className="flex items-center space-x-2 pt-1">
              <span className="text-xs font-semibold text-[#6E6E73]">AQI {telemetry.aqi}</span>
              <span className={`text-[11px] font-semibold px-2.5 py-0.5 rounded-full ${ambEpa.bg}`}>
                {ambEpa.label}
              </span>
            </div>
          </div>

          {/* Clean Delta Bridge */}
          <div className="lg:col-span-2 flex flex-col items-center justify-center py-2 lg:py-0">
            <div className="flex items-center space-x-2 text-[#0A84FF] bg-[#F5F5F7] px-4 py-2 rounded-2xl border border-black/[0.04]">
              <span className="text-xs font-bold tracking-tight">−{reduction}</span>
              <ArrowRight className="w-4 h-4 stroke-[2.5]" />
            </div>
            <span className="text-[11px] text-[#6E6E73] font-medium mt-1.5">Delivered clean</span>
          </div>

          {/* Delivered Breathing Zone Air */}
          <div className="lg:col-span-5 space-y-2 lg:pl-4">
            <div className="text-xs font-medium text-[#6E6E73] flex items-center space-x-1.5">
              <span>Helmet breathing zone</span>
              <CheckCircle2 className="w-3.5 h-3.5 text-[#34C759]" />
            </div>

            <div className="flex items-baseline space-x-2">
              <span className="text-6xl sm:text-7xl font-bold tracking-tight text-[#34C759]">
                {telemetry.pm25_outlet}
              </span>
              <span className="text-sm font-medium text-[#6E6E73]">
                &mu;g/m&sup3; <span className="text-xs text-[#6E6E73]/70 font-normal">PM2.5</span>
              </span>
            </div>

            <div className="flex items-center space-x-2 pt-1">
              <span className="text-xs font-semibold text-[#6E6E73]">Delivered air</span>
              <span className="text-[11px] font-semibold px-2.5 py-0.5 rounded-full bg-[#34C759]/10 text-[#34C759]">
                Clean &bull; Safe to breathe
              </span>
            </div>
          </div>
        </div>

        {/* Secondary Metric Footnote */}
        <div className="mt-8 pt-5 border-t border-black/[0.05] grid grid-cols-2 sm:grid-cols-3 gap-4 text-xs">
          <div>
            <span className="text-[#6E6E73]">Ambient PM10:</span>{' '}
            <span className="font-semibold text-[#1D1D1F]">{telemetry.pm10_ambient} &mu;g/m&sup3;</span>
          </div>
          <div>
            <span className="text-[#6E6E73]">Delivered PM10:</span>{' '}
            <span className="font-semibold text-[#34C759]">{telemetry.pm10_outlet} &mu;g/m&sup3;</span>
          </div>
          <div className="col-span-2 sm:col-span-1">
            <span className="text-[#6E6E73]">Purified airflow:</span>{' '}
            <span className="font-semibold text-[#1D1D1F]">Adaptive 2–8 L/min</span>
          </div>
        </div>
      </div>
    </div>
  );
};
