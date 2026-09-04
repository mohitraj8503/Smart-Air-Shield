'use client';

import React from 'react';
import { ShieldCheck, Wind, AlertTriangle } from 'lucide-react';
import { TelemetryData } from '../types';

interface LiveAQIProps {
  telemetry: TelemetryData | null;
}

export const LiveAQI: React.FC<LiveAQIProps> = ({ telemetry }) => {
  if (!telemetry) {
    return (
      <div className="bg-slate-900/60 border border-slate-800/80 rounded-2xl p-6 text-center text-slate-400">
        <p>No telemetry received yet. Connect your SMART AIR-SHIELD to view live air quality data.</p>
      </div>
    );
  }

  const getAqiDetails = (aqi: number) => {
    if (aqi <= 50) return { label: 'Good', bg: 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30', color: '#10b981' };
    if (aqi <= 100) return { label: 'Moderate', bg: 'bg-amber-500/10 text-amber-400 border-amber-500/30', color: '#f59e0b' };
    if (aqi <= 150) return { label: 'Sensitive Groups', bg: 'bg-orange-500/10 text-orange-400 border-orange-500/30', color: '#f97316' };
    if (aqi <= 200) return { label: 'Unhealthy', bg: 'bg-rose-500/10 text-rose-400 border-rose-500/30', color: '#ef4444' };
    if (aqi <= 300) return { label: 'Very Unhealthy', bg: 'bg-purple-500/10 text-purple-400 border-purple-500/30', color: '#8b5cf6' };
    return { label: 'Hazardous', bg: 'bg-red-950 text-red-300 border-red-800', color: '#7f1d1d' };
  };

  const aqiInfo = getAqiDetails(telemetry.aqi);

  return (
    <div className="space-y-4">
      {/* Top Banner: EPA AQI & Filtration Efficiency */}
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        {/* AQI Overview Card */}
        <div className="bg-slate-900/80 border border-slate-800/90 rounded-2xl p-5 shadow-lg flex items-center justify-between">
          <div>
            <div className="text-xs font-semibold text-slate-400 tracking-wider uppercase mb-1">
              Ambient US EPA AQI
            </div>
            <div className="flex items-baseline space-x-3">
              <span className="text-5xl font-black tracking-tight text-white">{telemetry.aqi}</span>
              <span className={`text-xs font-bold px-3 py-1 rounded-full border ${aqiInfo.bg}`}>
                {aqiInfo.label}
              </span>
            </div>
            <div className="text-xs text-slate-400 mt-2 flex items-center space-x-1">
              <AlertTriangle className="w-3.5 h-3.5 text-amber-400" />
              <span>Real-time roadside air hazard rating</span>
            </div>
          </div>

          <div className="relative w-20 h-20 flex items-center justify-center">
            <svg className="w-full h-full transform -rotate-90" viewBox="0 0 36 36">
              <path
                className="text-slate-800"
                strokeWidth="3.5"
                stroke="currentColor"
                fill="none"
                d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831"
              />
              <path
                strokeWidth="3.5"
                strokeDasharray={`${Math.min(100, (telemetry.aqi / 300) * 100)}, 100`}
                strokeLinecap="round"
                stroke={aqiInfo.color}
                fill="none"
                d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831"
              />
            </svg>
            <div className="absolute text-center">
              <Wind className="w-5 h-5 text-slate-300 mx-auto" />
            </div>
          </div>
        </div>

        {/* Filtration Efficiency Proof of Concept Card */}
        <div className="bg-gradient-to-br from-emerald-950/40 via-slate-900/90 to-cyan-950/40 border border-emerald-500/30 rounded-2xl p-5 shadow-lg flex items-center justify-between">
          <div>
            <div className="text-xs font-semibold text-emerald-400 tracking-wider uppercase mb-1 flex items-center space-x-1.5">
              <ShieldCheck className="w-4 h-4 text-emerald-400" />
              <span>3-Stage HEPA H13 Efficiency</span>
            </div>
            <div className="flex items-baseline space-x-2">
              <span className="text-5xl font-black text-emerald-300 tracking-tight">
                {telemetry.filtrationEfficiency}%
              </span>
              <span className="text-xs font-semibold text-emerald-400/80">Particulates Filtered</span>
            </div>
            <div className="text-xs text-slate-300 mt-2">
              Target: <span className="font-semibold text-emerald-400">&ge; 95%</span> (Standard breathing zone delivery)
            </div>
          </div>

          <div className="hidden sm:flex flex-col items-center bg-emerald-500/10 border border-emerald-500/20 px-3 py-2 rounded-xl">
            <span className="text-[10px] uppercase text-emerald-400 font-bold">Reduction</span>
            <span className="text-xl font-bold text-white">
              {Math.max(0, telemetry.pm25_ambient - telemetry.pm25_outlet)}
            </span>
            <span className="text-[10px] text-slate-400">&mu;g/m&sup3;</span>
          </div>
        </div>
      </div>

      {/* Side-by-Side Dual Sensor Comparison (Ambient vs Delivered Air) */}
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        {/* Ambient Inlet Sensor Card */}
        <div className="bg-slate-900/90 border border-rose-500/20 rounded-2xl p-5 relative overflow-hidden">
          <div className="absolute top-0 right-0 w-24 h-24 bg-rose-500/5 rounded-full blur-2xl pointer-events-none" />
          <div className="flex items-center justify-between mb-3">
            <span className="text-xs font-bold uppercase tracking-wider text-rose-400 flex items-center space-x-1.5">
              <span className="w-2 h-2 rounded-full bg-rose-500 animate-pulse" />
              <span>Ambient Roadside Air (Inlet)</span>
            </span>
            <span className="text-[11px] text-slate-400">PMS7003 #1</span>
          </div>

          <div className="grid grid-cols-2 gap-3 mt-4">
            <div className="bg-slate-950/60 border border-slate-800/80 rounded-xl p-3 text-center">
              <div className="text-xs text-slate-400 font-medium">PM 2.5</div>
              <div className="text-3xl font-extrabold text-white mt-1">
                {telemetry.pm25_ambient}
              </div>
              <div className="text-[11px] text-slate-500 mt-0.5">&mu;g/m&sup3;</div>
            </div>

            <div className="bg-slate-950/60 border border-slate-800/80 rounded-xl p-3 text-center">
              <div className="text-xs text-slate-400 font-medium">PM 10</div>
              <div className="text-3xl font-extrabold text-white mt-1">
                {telemetry.pm10_ambient}
              </div>
              <div className="text-[11px] text-slate-500 mt-0.5">&mu;g/m&sup3;</div>
            </div>
          </div>
        </div>

        {/* Outlet Delivered Clean Air Card */}
        <div className="bg-slate-900/90 border border-cyan-500/20 rounded-2xl p-5 relative overflow-hidden">
          <div className="absolute top-0 right-0 w-24 h-24 bg-cyan-500/5 rounded-full blur-2xl pointer-events-none" />
          <div className="flex items-center justify-between mb-3">
            <span className="text-xs font-bold uppercase tracking-wider text-cyan-400 flex items-center space-x-1.5">
              <span className="w-2 h-2 rounded-full bg-cyan-400" />
              <span>Breathing Zone Delivered Air</span>
            </span>
            <span className="text-[11px] text-slate-400">PMS7003 #2 (Duct)</span>
          </div>

          <div className="grid grid-cols-2 gap-3 mt-4">
            <div className="bg-slate-950/60 border border-slate-800/80 rounded-xl p-3 text-center">
              <div className="text-xs text-slate-400 font-medium">PM 2.5 Clean</div>
              <div className="text-3xl font-extrabold text-cyan-300 mt-1">
                {telemetry.pm25_outlet}
              </div>
              <div className="text-[11px] text-slate-500 mt-0.5">&mu;g/m&sup3;</div>
            </div>

            <div className="bg-slate-950/60 border border-slate-800/80 rounded-xl p-3 text-center">
              <div className="text-xs text-slate-400 font-medium">PM 10 Clean</div>
              <div className="text-3xl font-extrabold text-cyan-300 mt-1">
                {telemetry.pm10_outlet}
              </div>
              <div className="text-[11px] text-slate-500 mt-0.5">&mu;g/m&sup3;</div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};
