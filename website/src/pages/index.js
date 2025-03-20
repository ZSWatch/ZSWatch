import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import HomepageFeatures from '@site/src/components/HomepageFeatures';
import SubscriptionForm from '@site/src/components/SubscriptionForm';

import styles from './index.module.css';

import StainlessWatchStand from '@site/static/img/real_photos/stainless_stand.JPG'
import StainlessWatchFrontWide from '@site/static/img/real_photos/stainless_front_wide.JPG'
import ClearWatchFrontWide from '@site/static/img/real_photos/clear_front_wide.JPG'


import ImageGallery from "react-image-gallery";
import "react-image-gallery/styles/css/image-gallery.css";

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
      <ImageGallery
        autoPlay
        showPlayButton={false}
        showFullscreenButton={false}
        showNav={false} items={images}
        additionalClass={styles.watchImg}
        slideDuration={800}
        slideInterval={3000}
        />
    );
}

function HomepageHeader() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <header className={clsx('hero hero--primary', styles.heroBanner)}>
      <h1></h1>
      <div className={styles.container}>
        <div className="container">
          <div className="row">
            <div className={clsx('col col--6')}>
              <img src={require('@site/static/img/zswatch_social_card.png').default} alt="Logo" className={styles.imgLogo} />
              <p className="hero__subtitle">{siteConfig.tagline}</p>
              <div className={styles.buttons}>
                <SubscriptionForm />
              </div>
            </div>
            <div className={clsx('col col--6')}>
              <div style={{ display: 'flex', justifyContent: 'center' }}>
                <WatchImagesGallery />
              </div>
            </div>
          </div>
        </div>
      </div>
    </header>
  );
}

export default function Home() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <Layout
      title={`${siteConfig.title}`}
      description="ZSWatch is an Open Source Smartwatch built from scratch.">
      <HomepageHeader />
      <main>
        <HomepageFeatures />
      </main>
    </Layout>
  );
}
