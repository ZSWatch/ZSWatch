/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./src/**/*.{js,jsx,ts,tsx,md,mdx}",
    "./docs/**/*.{js,jsx,ts,tsx,md,mdx}",
    "./blog/**/*.{js,jsx,ts,tsx,md,mdx}",
  ],
  darkMode: ['class', '[data-theme="dark"]'], // Support both class and Docusaurus data-theme
  theme: {
    extend: {
      colors: {
        'zswatch-primary': '#FFBAAF',
        'zswatch-secondary': '#9EC8F6',
        'zswatch-dark': '#495060',
        'zswatch-darker': '#30343F',
      },
    },
  },
  plugins: [],
  corePlugins: {
    preflight: false, // disable Tailwind's base reset to avoid conflicts with Docusaurus
  },
}

