import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "SMART AIR-SHIELD — Clean Breathing Companion",
  description: "Apple-inspired live telemetry dashboard for SMART AIR-SHIELD helmet air purification module",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body className="bg-[#F5F5F7] text-[#1D1D1F] min-h-screen antialiased selection:bg-[#0A84FF]/20 selection:text-[#0A84FF]">
        {children}
      </body>
    </html>
  );
}
