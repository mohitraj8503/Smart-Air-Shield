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
      <div className="bg-white rounded-3xl p-8 sm:p-10 border border-black/[0.06] shadow-[0_2px_16px_rgba(0,0,0,0.04)] text-left">
        <div className="flex items-center space-x-3 mb-3">
          <div className="w-3 h-3 rounded-full bg-[#6E6E73]/40" />
          <span className="text-sm font-medium text-[#6E6E73] tracking-normal">Awaiting connection</span>
        </div>
        <h2 className="text-3xl font-semibold text-[#1D1D1F] tracking-tight">Breathing zone protection</h2>
        <p className="text-base text-[#6E6E73] mt-2 max-w-2xl leading-relaxed">
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
      {/* Hero Card: Zoomed In Boldness */}
      <div className="bg-white rounded-3xl p-6 sm:p-10 border border-black/[0.06] shadow-[0_4px_24px_rgba(0,0,0,0.04)] relative overflow-hidden">
        {/* Top Header Bar */}
        <div className="flex flex-wrap items-center justify-between gap-4 mb-8 pb-6 border-b border-black/[0.05]">
          <div className="flex items-center space-x-3">
            <span className="relative flex h-3 w-3">
              <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-[#34C759] opacity-75"></span>
              <span className="relative inline-flex rounded-full h-3 w-3 bg-[#34C759]"></span>
            </span>
            <span className="text-sm font-semibold text-[#1D1D1F] tracking-normal">
              Active filtration
            </span>
            <span className="text-sm text-[#6E6E73]">&bull; 3-stage HEPA H13</span>
          </div>

          {/* Large Efficiency Pill */}
          <div className="inline-flex items-center space-x-2 px-4 py-1.5 rounded-full bg-[#34C759]/10 text-[#34C759] text-sm font-bold">
            <ShieldCheck className="w-4 h-4" />
            <span>{telemetry.filtrationEfficiency}% particulate removal</span>
          </div>
        </div>

        {/* The Hero Comparison Readout (Zoomed-in confident numerals) */}
        <div className="grid grid-cols-1 lg:grid-cols-12 gap-8 items-center">
          {/* Ambient External Air */}
          <div className="lg:col-span-5 space-y-3">
            <div className="text-sm font-medium text-[#6E6E73]">
              Outside ambient air
            </div>
            <div className="flex items-baseline space-x-3">
              <span className="text-7xl sm:text-8xl md:text-9xl font-extrabold tracking-tight text-[#1D1D1F]">
                {telemetry.pm25_ambient}
              </span>
              <span className="text-base font-semibold text-[#6E6E73]">
                &mu;g/m&sup3; <span className="text-xs text-[#6E6E73]/70 font-normal block">PM2.5</span>
              </span>
            </div>

            <div className="flex items-center space-x-2.5 pt-1">
              <span className="text-sm font-bold text-[#6E6E73]">AQI {telemetry.aqi}</span>
              <span className={`text-xs font-semibold px-3 py-1 rounded-full ${ambEpa.bg}`}>
                {ambEpa.label}
              </span>
            </div>
          </div>

          {/* Clean Delta Bridge */}
          <div className="lg:col-span-2 flex flex-col items-center justify-center py-4 lg:py-0">
            <div className="flex items-center space-x-2 text-[#0A84FF] bg-[#F5F5F7] px-5 py-2.5 rounded-2xl border border-black/[0.04] shadow-sm">
              <span className="text-base font-bold tracking-tight">−{reduction}</span>
              <ArrowRight className="w-5 h-5 stroke-[2.5]" />
            </div>
            <span className="text-xs text-[#6E6E73] font-medium mt-2">Delivered clean</span>
          </div>

          {/* Delivered Breathing Zone Air */}
          <div className="lg:col-span-5 space-y-3 lg:pl-6">
            <div className="text-sm font-medium text-[#6E6E73] flex items-center space-x-1.5">
              <span>Helmet breathing zone</span>
              <CheckCircle2 className="w-4 h-4 text-[#34C759]" />
            </div>

            <div className="flex items-baseline space-x-3">
              <span className="text-7xl sm:text-8xl md:text-9xl font-extrabold tracking-tight text-[#34C759]">
                {telemetry.pm25_outlet}
              </span>
              <span className="text-base font-semibold text-[#6E6E73]">
                &mu;g/m&sup3; <span className="text-xs text-[#6E6E73]/70 font-normal block">PM2.5</span>
              </span>
            </div>

            <div className="flex items-center space-x-2.5 pt-1">
              <span className="text-sm font-bold text-[#6E6E73]">Delivered air</span>
              <span className="text-xs font-semibold px-3 py-1 rounded-full bg-[#34C759]/10 text-[#34C759]">
                Clean &bull; Safe to breathe
              </span>
            </div>
          </div>
        </div>

        {/* Secondary Metric Footnote */}
        <div className="mt-10 pt-6 border-t border-black/[0.05] grid grid-cols-2 sm:grid-cols-3 gap-6 text-sm">
          <div>
            <span className="text-[#6E6E73]">Ambient PM10:</span>{' '}
            <span className="font-bold text-[#1D1D1F] text-base ml-1">{telemetry.pm10_ambient} &mu;g/m&sup3;</span>
          </div>
          <div>
            <span className="text-[#6E6E73]">Delivered PM10:</span>{' '}
            <span className="font-bold text-[#34C759] text-base ml-1">{telemetry.pm10_outlet} &mu;g/m&sup3;</span>
          </div>
          <div className="col-span-2 sm:col-span-1">
            <span className="text-[#6E6E73]">Purified airflow:</span>{' '}
            <span className="font-bold text-[#1D1D1F] text-base ml-1">Adaptive 2–8 L/min</span>
          </div>
        </div>
      </div>
    </div>
  );
};
