import Link from '@docusaurus/Link';
import styles from './styles.module.css';

export default function SubscriptionForm() {
  return (
    <form method="POST" action="https://8624cdd8.sibforms.com/serve/MUIFALM3Ssztzpct9-Ej_L1x-ZTpeqPjztgSukyA3NeuAURXXnlQtI1sd8dvZ4GFrtHP_5vGwH_TJHFHG5kQ_FlO1O5J6fz1zUh3nGnViH3947txULamjrEPZ6SE_XyGRy87LPqAkTlzg2mo6aeo6kHvMU2BSl23qZasOs0FtaieMQK_w4zqdbKjrlpp_JN4XS2ksl_xKksL9t3eOg==" className={styles.signup}>
      <div className={styles.center}>
        <div className={styles.signupTitle}>Sign up for updates here</div>
        <div>
          <input type="email" name="EMAIL" required placeholder="Enter your email" className={styles.emailInput} />
          <input type="submit" value="Subscribe" className={styles.subscribeBtn} />
        </div>
        <Link to="/blog" className={styles.blogReminder}>
          Read the blog for latest info
        </Link>
        <input type="text" name="email_address_check" value="" className="input--hidden" style={{ display: 'none' }} />
        <input type="hidden" name="locale" value="en" />
      </div>
    </form>
  );
}
