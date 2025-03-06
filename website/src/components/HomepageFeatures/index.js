import clsx from 'clsx';
import Heading from '@theme/Heading';
import styles from './styles.module.css';

import ZephyrLogo from '@site/static/img/zephyr_color.svg';

const FeatureList = [
  {
    title: 'Open Source Hardware',
    image: <img className={styles.featureSvg} role="img" src={require('@site/static/img/Hardware_Overview_transparent.png').default} />,
    description: (
      <>
        ZSWatch is designed from the ground up to be open source.
        Designed in Open Source KiCad, designed to be easily modified and expanded.
      </>
    ),
  },
  {
    title: 'Open Source Software',
    image: <img className={styles.featureSvg} role="img" src={require('@site/static/img/ui_rund_demo_watchface.png').default} />,
    description: (
      <>
        Docusaurus lets you focus on your docs, and we&apos;ll do the chores. Go
        ahead and move your docs into the <code>docs</code> directory.
      </>
    ),
  },
  {
    title: 'Powered by Zephyr',
    image: <ZephyrLogo className={styles.featureSvg} role="img" />,
    description: (
      <>
        Zephyr is a new generation, scalable, optimized, secure RTOS. Very feature risch and, making it ideal for IoT and wearable devices.
      </>
    ),
  },
];

function Feature({image, title, description}) {
  return (
    <div className={clsx('col col--4')}>
      <div className="text--center">
        {image}
      </div>
      <div className="text--center padding-horiz--md">
        <Heading as="h3">{title}</Heading>
        <p>{description}</p>
      </div>
    </div>
  );
}

export default function HomepageFeatures() {
  return (
    <section className={styles.features}>
      <div className="container">
        <div className="row">
          {FeatureList.map((props, idx) => (
            <Feature key={idx} {...props} />
          ))}
        </div>
      </div>
    </section>
  );
}
