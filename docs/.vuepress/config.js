module.exports = {
  markdown: {
    lineNumbers: true
  },
  title: 'Theengs app',
  description: 'Multi platform BLE Application leveraging Theengs Decoder',
  head: [
    ['link', { rel: "apple-touch-icon", sizes: "180x180", href: ".apple-touch-icon.png"}],
    ['link', { rel: "icon", type: "image/png", sizes: "32x32", href: "/favicon-32x32.png"}],
    ['link', { rel: "icon", type: "image/png", sizes: "16x16", href: "/favicon-16x16.png"}],
    ['link', { rel: 'manifest', href: '/manifest.json' }],
    ['meta', { name: 'theme-color', content: '#3eaf7c' }],
    ['meta', { name: 'apple-mobile-web-app-capable', content: 'yes' }],
    ['meta', { name: 'apple-mobile-web-app-status-bar-style', content: 'black' }],
    ['link', { rel: 'mask-icon', href: '/icons/safari-pinned-tab.svg', color: '#3eaf7c' }],
    ['meta', { name: 'msapplication-TileImage', content: '/favicon-144x144.png' }],
    ['meta', { name: 'msapplication-TileColor', content: '#000000' }]
  ],
  themeConfig: {
    smoothScroll: true,
    repo: 'theengs/app',
    docsDir: 'docs',
    docsBranch: 'development',
    lastUpdated: 'Last Updated',
    editLinks: false,
    nav: [
      { text: 'Home', link: 'https://www.theengs.io', target:'_self', rel:''},
      { text: 'App', link: 'https://app.theengs.io', target:'_self', rel:''},
      { text: 'Gateway', link: 'https://gateway.theengs.io', target:'_self', rel:''},
      { text: 'Decoder', link: 'https://decoder.theengs.io', target:'_self', rel:''}
    ],
    sidebar: [
      ['/','0 - What is it for 🏠'],
      {
        title: '1 - Prerequisites🧭',
        children: [
          'prerequisites/devices',
          'prerequisites/controller']
      },
      ['/install/install','2 - Install ✔️'],
      {
        title: '3 - Use ✈️',
        children: [
          'use/use',
          'use/privacy'
        ]
      },
      {
        title: '4 - Participate 💻',
        children: [
          'participate/adding-decoders',
          'participate/build',
          'participate/support',
          'participate/development'
        ]
      }
  ]
  },
  plugins: [
    ['@vuepress/pwa', {
      serviceWorker: true,
      updatePopup: true
    }],
    ['@vuepress/medium-zoom', true],
    ['@vuepress/nprogress']
  ]
}