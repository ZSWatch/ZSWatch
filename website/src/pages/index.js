import React, { useEffect } from 'react';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import Link from '@docusaurus/Link';

import StainlessWatchFrontWide from '@site/static/img/real_photos/stainless_front_wide.JPG';
import StainlessWatchStand from '@site/static/img/real_photos/stainless_stand.JPG';
import ClearWatchFrontWide from '@site/static/img/real_photos/clear_front_wide.JPG';
import HardwareOverview from '@site/static/img/Hardware_Overview_transparent.png';
import UIDemo from '@site/static/img/ui_rund_demo_watchface.png';
import ZSWatchLogo from '@site/static/img/ZSWatch_Logo.svg';
import ZSWatchText from '@site/static/img/ZSWatch_Text.svg';
import ZephyrLogo from '@site/static/img/zephyr_color.svg';

import ImageGallery from "react-image-gallery";
import "react-image-gallery/styles/css/image-gallery.css";
import '../css/watch-gallery.css';

const images = [
  {
    original: StainlessWatchFrontWide,
  },
  {
    original: StainlessWatchStand,
  },
  {
    original: ClearWatchFrontWide,
  },
];

function WatchImagesGallery() {
  return (
    <div className="w-full max-w-sm lg:max-w-sm md:max-w-md sm:max-w-full mx-auto sm-auto">
      <ImageGallery
        autoPlay
        showPlayButton={false}
        showFullscreenButton={false}
        showNav={false}
        items={images}
        slideDuration={800}
        slideInterval={3000}
      />
    </div>
  );
}

function SubscriptionForm() {
  return (
    <form method="POST" action="https://8624cdd8.sibforms.com/serve/MUIFALM3Ssztzpct9-Ej_L1x-ZTpeqPjztgSukyA3NeuAURXXnlQtI1sd8dvZ4GFrtHP_5vGwH_TJHFHG5kQ_FlO1O5J6fz1zUh3nGnViH3947txULamjrEPZ6SE_XyGRy87LPqAkTlzg2mo6aeo6kHvMU2BSl23qZasOs0FtaieMQK_w4zqdbKjrlpp_JN4XS2ksl_xKksL9t3eOg==" className="flex gap-3 mb-4">
      <input type="text" name="email_address_check" value="" style={{ display: 'none' }} />
      <input type="hidden" name="locale" value="en" />
      <input
        type="email"
        name="EMAIL"
        required
        placeholder="Enter your email for updates"
        className="flex-1 text-white"
        style={{
          padding: '12px 16px',
          border: '1px solid #495060',
          borderRadius: '10px',
          backgroundColor: 'rgba(48, 52, 63, 0.6)',
          color: '#f8fafc',
          fontSize: '14px',
          outline: 'none'
        }}
      />
      <button
        type="submit"
        className="px-6 py-3 font-semibold rounded-lg transition-colors duration-200"
        style={{
          backgroundColor: '#9EC8F6',
          color: '#30343F',
          border: 'none',
          fontWeight: '600'
        }}
        onMouseEnter={(e) => e.target.style.backgroundColor = '#7db4f0'}
        onMouseLeave={(e) => e.target.style.backgroundColor = '#9EC8F6'}
      >
        Subscribe
      </button>
    </form>
  );
}

function HomepageHeader() {
  return (
    <div className="relative overflow-hidden w-full">
      {/* Decorative background element */}
      <div className="absolute top-1/4 -right-10 w-96 h-96 bg-blue-400/10 rounded-full blur-3xl"></div>
      
      <div className="relative z-10 px-4 sm:px-6 lg:px-8 w-full">
        {/* Hero Section */}
        <section className="grid grid-cols-1 lg:grid-cols-2 gap-6 items-center py-4 px-5 min-h-[50vh] max-w-6xl mx-auto">
          <div className="space-y-3">
            {/* Logo and Title */}
            <div className="flex items-center gap-4 mb-2">
              <ZSWatchLogo style={{width: "20%", height: "20%"}}/>
              <ZSWatchText style={{height: "50%", width: "65%"}}/>
            </div>
            
            <p className="text-base text-gray-300 leading-relaxed">
              The Open Source Zephyr™ based Smartwatch. Built from scratch with complete hardware and software transparency.
            </p>
            
            <SubscriptionForm />
            
            <div className="mb-2">
              <Link to="/blog" className="font-medium transition-colors text-sm" style={{color: '#9EC8F6', textDecoration: 'none'}}>
                Read the blog for latest info →
              </Link>
            </div>

            <div className="flex gap-3 mb-3">
              <Link
                to="/docs/intro"
                className="px-5 py-2.5 font-semibold rounded-xl hover:-translate-y-0.5 transition-all duration-200 text-sm" style={{
                  background: '#FFBAAF',
                  color: '#30343F',
                  boxShadow: '0 4px 14px 0 rgba(255, 186, 175, 0.4)'
                }}
              >
                Get Started
              </Link>
              <Link
                to="/docs/intro"
                className="px-5 py-2.5 text-white font-semibold rounded-xl transition-all duration-200 text-sm no-underline" href="/docs/intro" style={{
                  backgroundColor: 'transparent',
                  border: '1px solid #9EC8F6',
                  textDecoration: 'none',
                  display: 'inline-block'
                }}
                onMouseEnter={(e) => {
                  e.target.style.borderColor = '#9EC8F6';
                  e.target.style.backgroundColor = 'rgba(158, 200, 246, 0.1)';
                  e.target.style.textDecoration = 'none';
                }}
                onMouseLeave={(e) => {
                  e.target.style.borderColor = '#495060';
                  e.target.style.backgroundColor = 'transparent';
                  e.target.style.textDecoration = 'none';
                }}
              >
                View Documentation
              </Link>
            </div>
            
            {/* Stats */}
            <div className="flex gap-6 mt-2">
              <div>
                <div className="text-xl font-bold mb-0.5" style={{color: '#9EC8F6', fontSize: '1.5rem'}}>100%</div>
                <div className="text-sm" style={{color: '#64748b', fontSize: '0.9rem'}}>Open Source</div>
              </div>
              <div>
                <div className="text-xl font-bold mb-0.5" style={{color: '#9EC8F6', fontSize: '1.5rem'}}>C</div>
                <div className="text-sm" style={{color: '#64748b', fontSize: '0.9rem'}}>Native Code</div>
              </div>
              <div>
                <div className="text-xl font-bold mb-0.5" style={{color: '#9EC8F6', fontSize: '1.5rem'}}>Zephyr</div>
                <div className="text-sm" style={{color: '#64748b', fontSize: '0.9rem'}}>RTOS Powered</div>
              </div>
            </div>
          </div>

          {/* Watch Image Gallery */}
          <div className="px-0">
            <WatchImagesGallery />
          </div>
        </section>
      </div>
    </div>
  );
}

function FeaturesSection() {
  const BORDER_COLORS = {
    default: '#495060',
    hover: '#9EC8F6'
  };

  const features = [
    {
      title: 'Open Source Hardware',
      image: HardwareOverview,
      description: 'ZSWatch is designed from the ground up to be open source. Designed in Open Source KiCad, designed to be easily modified and expanded.'
    },
    {
      title: 'Open Source Software',
      image: UIDemo,
      description: 'Written from scratch in C, there are no binary blobs. Everything from high level application to the lowest level Bluetooth radio driver is open source.'
    },
    {
      title: 'Powered by Zephyr',
      image: ZephyrLogo,
      description: 'Zephyr is a new generation, scalable, optimized, secure RTOS. Very feature rich and, making it ideal for IoT and wearable devices.'
    }
  ];

  return (
    <section className="py-6 min-h-[50vh]">
      <div className="max-w-6xl mx-auto px-5">
        <p className="text-lg text-gray-300 text-center max-w-2xl mx-auto mb-6">
          Complete transparency from hardware design to firmware implementation. No black boxes, no proprietary blobs.
        </p>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6 px-8">
          {features.map((feature, idx) => (
            <div 
              key={idx} 
              className="bg-gradient-to-br from-gray-800/60 to-gray-800/40 rounded-xl p-8 text-center hover:-translate-y-1 transition-all duration-200 flex flex-col h-full"
              style={{
                border: `1px solid ${BORDER_COLORS.default}`,
                minHeight: '320px'
              }}
              onMouseEnter={(e) => {
                e.target.style.borderColor = BORDER_COLORS.hover;
              }}
              onMouseLeave={(e) => {
                e.target.style.borderColor = BORDER_COLORS.default;
              }}
            >
              <div className="w-full h-48 flex items-center justify-center mb-6">
                {feature.title === 'Powered by Zephyr' ? (
                  <ZephyrLogo className="w-full h-full object-contain" />
                ) : (
                  <img 
                    src={feature.image} 
                    alt={feature.title}
                    className="w-full h-full object-contain"
                  />
                )}
              </div>
              <div className="flex-1 flex flex-col">
                <h3 className="text-base font-semibold mb-1.5 text-white">{feature.title}</h3>
                <p className="text-gray-300 leading-relaxed text-xs flex-1">{feature.description}</p>
              </div>
            </div>
          ))}
        </div>
      </div>
    </section>
  );
}

export default function Home() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <div className="zswatch-background-gradient-always min-h-screen">
      <Layout
        title={siteConfig.title}
        description="ZSWatch is an Open Source Smartwatch built from scratch.">
        <HomepageHeader />
        <main>
          <FeaturesSection />
        </main>
      </Layout>
    </div>
  );
}