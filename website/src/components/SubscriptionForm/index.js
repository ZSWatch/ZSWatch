
import styles from './styles.module.css';

export default function SubscriptionForm() {
  return (
    <form method="post" action="https://zswatch.pikapod.net/subscription/form" className={styles.signup}>
      <div className={styles.center}>
        <div className={styles.signupTitle}>Sign up for updates here</div>
        <input type="hidden" name="nonce" />
        <div>
          <input type="email" name="email" required placeholder="Enter your email" className={styles.emailInput} />
          <input type="submit" value="Subscribe" className={styles.subscribeBtn} />
        </div>
        <a href="/blog/" className={styles.blogReminder}>
          Read the blog for latest info
        </a>
        <p hidden>
          <input id="2cca0" type="checkbox" name="l" defaultChecked value="2cca0c50-ef0d-4339-b695-9e559f4a4359" />
          <label htmlFor="2cca0">ZSWatch Test List</label>
          <br />
        </p>
      </div>
    </form>
  );
}