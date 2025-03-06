import React, { useEffect, useState } from 'react';
import axios from 'axios';
import styles from './styles.module.css';
import GithubIcon from '@site/static/icons/github.svg';


const GitHubStars = (props) => {
  const [stars, setStars] = useState(null);

  useEffect(() => {
    const fetchStars = async () => {
      try {
        const response = await axios.get(`https://api.github.com/repos/${props.repoOrg}/${props.repoName}`);
        setStars(response.data.stargazers_count);
      } catch (error) {
        console.error('Error fetching GitHub stars:', error);
      }
    };

    fetchStars();
  }, []);

  return (
    <a href={`https://github.com/${props.repoOrg}/${props.repoName}`} className={styles.githubIconContainer}>
      <GithubIcon alt="GitHub" className={styles.githubIcon}/>
      {stars !== null ? `${Math.round((stars / 1000 + Number.EPSILON) * 10) / 10}K★` : '...'}
    </a>
  );
};

export default GitHubStars;