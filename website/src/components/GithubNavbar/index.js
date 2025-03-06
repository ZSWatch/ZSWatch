import React, { useEffect, useState } from 'react';
import axios from 'axios';
import styles from './styles.module.css';

const GitHubStars = () => {
  const [stars, setStars] = useState(null);

  useEffect(() => {
    const fetchStars = async () => {
      try {
        const response = await axios.get('https://api.github.com/repos/jakkra/ZSWatch');
        setStars(response.data.stargazers_count);
      } catch (error) {
        console.error('Error fetching GitHub stars:', error);
      }
    };

    fetchStars();
  }, []);

  return (
    <a href="https://github.com/jakkra/ZSWatch" className={styles.githubIconContainer}>
      <img src="/icons/github.svg" alt="GitHub" className={styles.githubIcon}/>
      {stars !== null ? `${Math.round((stars / 1000 + Number.EPSILON) * 10) / 10}K★` : '...'}
    </a>
  );
};

export default GitHubStars;