import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import HomepageFeatures from '@site/src/components/HomepageFeatures';

import styles from './index.module.css';

function HomepageHeader() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <header className={clsx('hero hero--primary', styles.heroBanner)}>
      <h1></h1>
      <div className="container">
        <img src={require('@site/static/img/zswatch_social_card.png').default} alt="Logo" className={styles.imgLogo} />
        <p className="hero__subtitle">{siteConfig.tagline}</p>
        <div className={styles.buttons}>
          <SubscriptionForm />
        </div>
      </div>
    </header>
  );
}

function SubscriptionForm() {
  return (
    <form method="post" action="https://zswatch.pikapod.net/subscription/form" className={styles.signup}>
      <div>
        <div className={styles.signupTitle}>Sign up for updates here</div>
        <input type="hidden" name="nonce" />
        <div>
          <input type="email" name="email" required placeholder="Enter your email" className={styles.emailInput} />
          <input type="submit" value="Subscribe" className={styles.subscribeBtn} />
        </div>
        <p hidden>
          <input id="2cca0" type="checkbox" name="l" defaultChecked value="2cca0c50-ef0d-4339-b695-9e559f4a4359" />
          <label htmlFor="2cca0">ZSWatch Test List</label>
          <br />
        </p>
      </div>
    </form>
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
