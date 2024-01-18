const commonConfig = require('./public/commonConfig');

module.exports = {
  ...commonConfig,
  title: 'Theengs BLE App: Vendor agnostic sensor reader',
  themeConfig: {
    repo: 'theengs/app',
    ...commonConfig.themeConfig,
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
  }
}
