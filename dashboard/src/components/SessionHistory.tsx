'use client';

import React from 'react';
import { ArrowDownToLine } from 'lucide-react';
import { HistoryPoint } from '../types';

interface SessionHistoryProps {
  history: HistoryPoint[];
}

export const SessionHistory: React.FC<SessionHistoryProps> = ({ history }) => {
  const exportCsv = () => {
    if (history.length === 0) return;

    const headers = 'time,ambient_pm25,delivered_pm25,blower_duty\n';
    const rows = history
      .map((pt) => `${pt.time},${pt.pm25_amb},${pt.pm25_out},${pt.duty}`)
      .join('\n');

    const blob = new Blob([headers + rows], { type: 'text/csv' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `smart-air-shield-${new Date().toISOString().slice(0, 10)}.csv`;
    a.click();
    URL.revokeObjectURL(url);
  };

  const width = 600;
  const height = 160;
  const padding = 24;

  const points = history.slice(-30);
  const maxVal = Math.max(100, ...points.map((p) => Math.max(p.pm25_amb, p.pm25_out)));

  const getX = (idx: number) => {
    if (points.length <= 1) return padding;
    return padding + (idx / (points.length - 1)) * (width - 2 * padding);
  };

  const getY = (val: number) => {
    return height - padding - (val / maxVal) * (height - 2 * padding);
  };

  const ambPath = points
    .map((p, i) => `${i === 0 ? 'M' : 'L'} ${getX(i)} ${getY(p.pm25_amb)}`)
    .join(' ');

  const outPath = points
    .map((p, i) => `${i === 0 ? 'M' : 'L'} ${getX(i)} ${getY(p.pm25_out)}`)
    .join(' ');

  return (
    <div className="bg-white rounded-3xl p-6 sm:p-7 border border-black/[0.06] shadow-[0_2px_14px_rgba(0,0,0,0.03)] space-y-4 text-left">
      <div className="flex items-center justify-between">
        <div>
          <div className="text-xs font-medium text-[#6E6E73]">
            Commute exposure history
          </div>
          <h3 className="text-lg font-semibold text-[#1D1D1F] tracking-tight mt-0.5">
            Particulate concentration over time
          </h3>
        </div>

        <button
          onClick={exportCsv}
          disabled={history.length === 0}
          className="inline-flex items-center space-x-1.5 px-3.5 py-1.5 rounded-xl bg-[#F5F5F7] hover:bg-[#E5E5EA] text-[#0A84FF] text-xs font-semibold transition-all disabled:opacity-40"
        >
          <ArrowDownToLine className="w-3.5 h-3.5" />
          <span>Export CSV</span>
        </button>
      </div>

      {/* Apple Health style minimalist chart */}
      <div className="w-full bg-[#F5F5F7]/70 rounded-2xl p-4 border border-black/[0.03]">
        <svg viewBox={`0 0 ${width} ${height}`} className="w-full h-36">
          {/* Subtle grid lines */}
          <line x1={padding} y1={getY(0)} x2={width - padding} y2={getY(0)} stroke="#E5E5EA" strokeWidth="1" />
          <line x1={padding} y1={getY(maxVal / 2)} x2={width - padding} y2={getY(maxVal / 2)} stroke="#E5E5EA" strokeWidth="1" strokeDasharray="3 3" />
          <line x1={padding} y1={getY(maxVal)} x2={width - padding} y2={getY(maxVal)} stroke="#E5E5EA" strokeWidth="1" strokeDasharray="3 3" />

          {/* Grid labels */}
          <text x={padding - 6} y={getY(0) + 3} fill="#6E6E73" fontSize="9" textAnchor="end">0</text>
          <text x={padding - 6} y={getY(maxVal / 2) + 3} fill="#6E6E73" fontSize="9" textAnchor="end">{Math.round(maxVal / 2)}</text>
          <text x={padding - 6} y={getY(maxVal) + 3} fill="#6E6E73" fontSize="9" textAnchor="end">{Math.round(maxVal)}</text>

          {/* Ambient line in subtle coral #FF3B30 */}
          {points.length > 0 && (
            <path d={ambPath} fill="none" stroke="#FF3B30" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" opacity="0.85" />
          )}

          {/* Delivered breathing air line in green #34C759 */}
          {points.length > 0 && (
            <path d={outPath} fill="none" stroke="#34C759" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round" />
          )}
        </svg>

        {/* Quiet Legend */}
        <div className="flex items-center justify-center space-x-6 pt-3 text-xs text-[#6E6E73]">
          <div className="flex items-center space-x-2">
            <span className="w-2.5 h-2.5 rounded-full bg-[#FF3B30]" />
            <span>Outside ambient PM2.5</span>
          </div>
          <div className="flex items-center space-x-2">
            <span className="w-2.5 h-2.5 rounded-full bg-[#34C759]" />
            <span className="font-medium text-[#1D1D1F]">Delivered clean air</span>
          </div>
        </div>
      </div>
    </div>
  );
};
