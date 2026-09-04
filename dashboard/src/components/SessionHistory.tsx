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

  const width = 800;
  const height = 200;
  const padding = 28;

  const points = history.slice(-40);
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
    <div className="bg-white rounded-3xl p-6 sm:p-8 border border-black/[0.06] shadow-[0_2px_16px_rgba(0,0,0,0.03)] space-y-5 text-left">
      <div className="flex items-center justify-between">
        <div>
          <div className="text-sm font-medium text-[#6E6E73]">
            Commute exposure history
          </div>
          <h3 className="text-xl font-bold text-[#1D1D1F] tracking-tight mt-0.5">
            Particulate concentration over time
          </h3>
        </div>

        <button
          onClick={exportCsv}
          disabled={history.length === 0}
          className="inline-flex items-center space-x-2 px-4 py-2 rounded-xl bg-[#F5F5F7] hover:bg-[#E5E5EA] text-[#0A84FF] text-sm font-semibold transition-all disabled:opacity-40 shadow-sm"
        >
          <ArrowDownToLine className="w-4 h-4" />
          <span>Export CSV</span>
        </button>
      </div>

      {/* Apple Health style expansive chart */}
      <div className="w-full bg-[#F5F5F7]/70 rounded-2xl p-5 border border-black/[0.03]">
        <svg viewBox={`0 0 ${width} ${height}`} className="w-full h-44 sm:h-52">
          {/* Subtle grid lines */}
          <line x1={padding} y1={getY(0)} x2={width - padding} y2={getY(0)} stroke="#E5E5EA" strokeWidth="1" />
          <line x1={padding} y1={getY(maxVal / 2)} x2={width - padding} y2={getY(maxVal / 2)} stroke="#E5E5EA" strokeWidth="1" strokeDasharray="3 3" />
          <line x1={padding} y1={getY(maxVal)} x2={width - padding} y2={getY(maxVal)} stroke="#E5E5EA" strokeWidth="1" strokeDasharray="3 3" />

          {/* Grid labels */}
          <text x={padding - 8} y={getY(0) + 4} fill="#6E6E73" fontSize="11" textAnchor="end">0</text>
          <text x={padding - 8} y={getY(maxVal / 2) + 4} fill="#6E6E73" fontSize="11" textAnchor="end">{Math.round(maxVal / 2)}</text>
          <text x={padding - 8} y={getY(maxVal) + 4} fill="#6E6E73" fontSize="11" textAnchor="end">{Math.round(maxVal)}</text>

          {/* Ambient line in subtle coral #FF3B30 */}
          {points.length > 0 && (
            <path d={ambPath} fill="none" stroke="#FF3B30" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round" opacity="0.85" />
          )}

          {/* Delivered breathing air line in green #34C759 */}
          {points.length > 0 && (
            <path d={outPath} fill="none" stroke="#34C759" strokeWidth="3" strokeLinecap="round" strokeLinejoin="round" />
          )}
        </svg>

        {/* Legend */}
        <div className="flex items-center justify-center space-x-8 pt-4 text-xs sm:text-sm text-[#6E6E73]">
          <div className="flex items-center space-x-2.5">
            <span className="w-3 h-3 rounded-full bg-[#FF3B30]" />
            <span>Outside ambient PM2.5</span>
          </div>
          <div className="flex items-center space-x-2.5">
            <span className="w-3 h-3 rounded-full bg-[#34C759]" />
            <span className="font-semibold text-[#1D1D1F]">Delivered clean air</span>
          </div>
        </div>
      </div>
    </div>
  );
};
