export interface TelemetryData {
  timestamp: number;
  pm25_ambient: number;
  pm10_ambient: number;
  pm25_outlet: number;
  pm10_outlet: number;
  aqi: number;
  bucket: number;
  duty: number;
  battery: number;
  mode: number; // 0: Off, 1: Auto, 2: Manual
  status: number;
  filtrationEfficiency: number;
}

export type ConnectionState = 'disconnected' | 'connecting' | 'connected' | 'mock';

export interface HistoryPoint {
  time: string;
  pm25_amb: number;
  pm25_out: number;
  duty: number;
}
