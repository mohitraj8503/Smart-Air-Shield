import type { Config } from "tailwindcss";

const config: Config = {
  content: [
    "./src/pages/**/*.{js,ts,jsx,tsx,mdx}",
    "./src/components/**/*.{js,ts,jsx,tsx,mdx}",
    "./src/app/**/*.{js,ts,jsx,tsx,mdx}",
  ],
  theme: {
    extend: {
      colors: {
        apple: {
          bg: "#F5F5F7",
          card: "#FFFFFF",
          text: "#1D1D1F",
          secondary: "#6E6E73",
          border: "rgba(0, 0, 0, 0.08)",
          segment: "#E5E5EA",
          blue: "#0A84FF",
        },
        epa: {
          good: "#34C759",
          moderate: "#FFCC00",
          usg: "#FF9500",
          unhealthy: "#FF3B30",
          veryUnhealthy: "#AF52DE",
          hazardous: "#8E2A2A",
        },
      },
      fontFamily: {
        sans: [
          "-apple-system",
          "BlinkMacSystemFont",
          '"SF Pro Display"',
          '"SF Pro Text"',
          '"Helvetica Neue"',
          "Inter",
          "sans-serif",
        ],
      },
      boxShadow: {
        apple: "0 2px 14px rgba(0, 0, 0, 0.04)",
        "apple-hover": "0 6px 20px rgba(0, 0, 0, 0.06)",
        segment: "0 1px 3px rgba(0, 0, 0, 0.12), 0 1px 2px rgba(0, 0, 0, 0.08)",
      },
    },
  },
  plugins: [],
};
export default config;
