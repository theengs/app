import { defineConfig } from 'vitepress'
import { commonNav, commonHead } from './commonConfig.mts'

export default defineConfig({
  title: 'Theengs BLE App: Vendor agnostic sensor reader',
  description: 'Theengs App reads Bluetooth Low Energy sensors and publishes their data to MQTT for smart-home integration.',
  base: '/',
  head: commonHead,
  ignoreDeadLinks: [/localhost/],
  lastUpdated: true,
  themeConfig: {
    siteTitle: 'Theengs App',
    nav: commonNav,
    socialLinks: [
      { icon: 'github', link: 'https://github.com/theengs/app' }
    ],
    editLink: {
      pattern: 'https://github.com/theengs/app/edit/development/docs/:path'
    },
    sidebar: [
      { text: '0 - What is it for 🏠', link: '/' },
      {
        text: '1 - Prerequisites 🧭',
        collapsed: true,
        items: [
          { text: 'Devices', link: '/prerequisites/devices' },
          { text: 'Controller', link: '/prerequisites/controller' }
        ]
      },
      { text: '2 - Install ✔️', link: '/install/install' },
      {
        text: '3 - Use ✈️',
        collapsed: true,
        items: [
          { text: 'Using the app', link: '/use/use' },
          { text: 'Privacy', link: '/use/privacy' }
        ]
      },
      {
        text: '4 - Participate 💻',
        collapsed: true,
        items: [
          { text: 'Adding decoders', link: '/participate/adding-decoders' },
          { text: 'Build', link: '/participate/build' },
          { text: 'Support', link: '/participate/support' },
          { text: 'Development', link: '/participate/development' }
        ]
      }
    ],
    search: { provider: 'local' },
    footer: {
      message: '<a href="https://github.com/theengs/home/blob/main/LICENSE">GPLv3 Licensed</a>',
      copyright: 'Copyright © 2026 Theengs'
    }
  },
  sitemap: { hostname: 'https://app.theengs.io' },
  vite: { publicDir: '.vitepress/public' }
})
