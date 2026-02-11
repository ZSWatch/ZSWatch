import React, {useCallback, useEffect, useMemo, useRef, useState} from 'react';
import Mermaid from '@theme/Mermaid';

import styles from './styles.module.css';

const ZoomableMermaid = ({value, title}) => {
  const [open, setOpen] = useState(false);
  const modalContentRef = useRef(null);
  const panZoomInstanceRef = useRef(null);

  const normalizedValue = useMemo(() => (value || '').trim(), [value]);

  const close = useCallback(() => setOpen(false), []);
  const openModal = useCallback(() => setOpen(true), []);

  useEffect(() => {
    if (typeof window === 'undefined') {
      return;
    }

    if (!open) {
      return;
    }

    const handleKeyDown = (e) => {
      if (e.key === 'Escape') {
        close();
      }
    };

    document.addEventListener('keydown', handleKeyDown);
    return () => document.removeEventListener('keydown', handleKeyDown);
  }, [open, close]);

  useEffect(() => {
    if (typeof window === 'undefined') {
      return;
    }

    if (!open) {
      if (panZoomInstanceRef.current) {
        panZoomInstanceRef.current.destroy();
        panZoomInstanceRef.current = null;
      }
      return;
    }

    const container = modalContentRef.current;
    if (!container) {
      return;
    }

    let cancelled = false;

    const tryInit = async () => {
      if (cancelled) {
        return;
      }

      const svg = container.querySelector('svg');
      if (!svg) {
        window.requestAnimationFrame(tryInit);
        return;
      }

      if (panZoomInstanceRef.current) {
        panZoomInstanceRef.current.destroy();
        panZoomInstanceRef.current = null;
      }

      const mod = await import('svg-pan-zoom');
      const svgPanZoom = mod.default ?? mod;

      panZoomInstanceRef.current = svgPanZoom(svg, {
        zoomEnabled: true,
        panEnabled: true,
        controlIconsEnabled: true,
        mouseWheelZoomEnabled: true,
        dblClickZoomEnabled: true,
        fit: true,
        center: true,
        minZoom: 0.2,
        maxZoom: 20,
      });

      panZoomInstanceRef.current.resize();
      panZoomInstanceRef.current.fit();
      panZoomInstanceRef.current.center();
    };

    window.requestAnimationFrame(tryInit);

    return () => {
      cancelled = true;
      if (panZoomInstanceRef.current) {
        panZoomInstanceRef.current.destroy();
        panZoomInstanceRef.current = null;
      }
    };
  }, [open, normalizedValue]);

  if (!normalizedValue) {
    return null;
  }

  return (
    <div className={styles.wrapper}>
      <div className={styles.preview} onClick={openModal} role="button" tabIndex={0}>
        <Mermaid value={normalizedValue} />
      </div>

      <div className={styles.actions}>
        <button type="button" className={styles.button} onClick={openModal}>
          Open zoomable view
        </button>
      </div>

      {open && (
        <div className={styles.modalOverlay} onMouseDown={close} role="dialog" aria-label={title || 'Diagram'}>
          <div className={styles.modal} onMouseDown={(e) => e.stopPropagation()}>
            <div className={styles.modalHeader}>
              <div className={styles.modalTitle}>{title || 'Diagram'}</div>
              <button type="button" className={styles.closeButton} onClick={close} aria-label="Close">
                Close
              </button>
            </div>
            <div className={styles.modalBody} ref={modalContentRef}>
              <Mermaid value={normalizedValue} />
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default ZoomableMermaid;
