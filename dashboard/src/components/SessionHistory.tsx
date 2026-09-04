'use client';

import React from 'react';
import { Download, TrendingDown, Activity } from 'lucide-react';
import { HistoryPoint } from '../types';

interface SessionHistoryProps {
  history: HistoryPoint[];
}

export const SessionHistory: React.FC<SessionHistoryProps> = ({ history }) => {
  const exportCsv = () => {
    if (history.length === 0) return;

    const headers = 'time,pm25_ambient,pm25_outlet,blower_duty\n';
    const rows = history
      .map((pt) => `${pt.time},${pt.pm25_amb},${pt.pm25_out},${pt.duty}`)
      .join('\n');

    const blob = new Blob([headers + rows], { type: 'text/csv' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `smart-air-shield-ride-${new Date().toISOString().slice(0, 19).replace(/:/g, '-')}.csv`;
    a.click();
    URL.revokeObjectURL(url);
  };

  // Compute SVG chart coordinates
  const width = 600;
  const height = 180;
  const padding = 30;

  const pointsToShow = history.slice(-30); // show last 30 data points
  const maxVal = Math.max(
    80,
    ...pointsToShow.map((p) => Math.max(p.pm25_amb, p.pm25_out))
  );

  const getX = (idx: number) => {
    if (pointsToShow.length <= 1) return padding;
    return padding + (idx / (pointsToShow.length - 1)) * (width - 2 * padding);
  };

  const getY = (val: number) => {
    return height - padding - (val / maxVal) * (height - 2 * padding);
  };

  const ambientPath = pointsToShow
    .map((pt, idx) => `${idx === 0 ? 'M' : 'L'} ${getX(idx)} ${getY(pt.pm25_amb)}`)
    .join(' ');

  const outletPath = pointsToShow
    .map((pt, idx) => `${idx === 0 ? 'M' : 'L'} ${getX(idx)} ${getY(pt.pm25_out)}`)
    .join(' ');

  return (
    <div className="bg-slate-900/80 border border-slate-800/90 rounded-2xl p-5 shadow-lg space-y-4">
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-2">
          <div className="p-2 rounded-xl bg-purple-500/10 text-purple-400">
            <Activity className="w-5 h-5" />
          </div>
          <div>
            <h3 className="font-bold text-white text-sm">Commute Particulate Exposure Trend</h3>
            <p className="text-xs text-slate-400">Ambient Roadside vs Breathing Zone Delivered Air</p>
          </div>
        </div>

        <button
          onClick={exportCsv}
          disabled={history.length === 0}
          className="py-1.5 px-3 bg-slate-800 hover:bg-slate-700 text-slate-200 text-xs font-semibold rounded-xl border border-slate-700 transition-all flex items-center space-x-1.5 disabled:opacity-40"
        >
          <Download className="w-3.5 h-3.5 text-cyan-400" />
          <span>Export CSV</span>
        </button>
      </div>

      {/* SVG Time-Series Chart */}
      <div className="w-full bg-slate-950/60 rounded-xl p-3 border border-slate-800/80 overflow-x-auto">
        <svg viewBox={`0 0 ${width} ${height}`} className="w-full h-44">
          {/* Grid lines */}
          <line x1={padding} y1={getY(0)} x2={width - padding} y2={getY(0)} stroke="#1e293b" strokeWidth="1" />
          <line x1={padding} y1={getY(maxVal / 2)} x2={width - padding} y2={getY(maxVal / 2)} stroke="#1e293b" strokeWidth="1" strokeDasharray="4 4" />
          <line x1={padding} y1={getY(maxVal)} x2={width - padding} y2={getY(maxVal)} stroke="#1e293b" strokeWidth="1" strokeDasharray="4 4" />

          {/* Grid labels */}
          <text x={padding - 5} y={getY(0)} fill="#64748b" fontSize="10" textAnchor="end">0</text>
          <text x={padding - 5} y={getY(maxVal / 2)} fill="#64748b" fontSize="10" textAnchor="end">{Math.round(maxVal / 2)}</text>
          <text x={padding - 5} y={getY(maxVal)} fill="#64748b" fontSize="10" textAnchor="end">{Math.round(maxVal)}</text>

          {/* Ambient PM2.5 Path (Rose) */}
          {pointsToShow.length > 0 && (
            <path d={ambientPath} fill="none" stroke="#f43f5e" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round" />
          )}

          {/* Outlet Delivered PM2.5 Path (Cyan) */}
          {pointsToShow.length > 0 && (
            <path d={outletPath} fill="none" stroke="#06b6d4" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round" />
          )}

          {/* Points */}
          {pointsToShow.map((pt, idx) => (
            <g key={idx}>
              <circle cx={getX(idx)} cy={getY(pt.pm25_amb)} r="2" fill="#f43f5e" />
              <circle cx={getX(idx)} cy={getY(pt.pm25_out)} r="2" fill="#06b6d4" />
            </g>
          ))}
        </svg>

        {/* Legend */}
        <div className="flex items-center justify-center space-x-6 pt-2 text-xs">
          <div className="flex items-center space-x-2">
            <span className="w-3 h-1 bg-rose-500 rounded-full" />
            <span className="text-slate-400">Ambient PM2.5 (&mu;g/m&sup3;)</span>
          </div>
          <div className="flex items-center space-x-2">
            <span className="w-3 h-1 bg-cyan-400 rounded-full" />
            <span className="text-slate-400">Breathing Zone PM2.5 (&mu;g/m&sup3;)</span>
          </div>
        </div>
      </div>
    </div>
  );
};
