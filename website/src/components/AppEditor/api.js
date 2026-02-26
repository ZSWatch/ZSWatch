// Copyright (c) 2025 ZSWatch Project
// SPDX-License-Identifier: Apache-2.0

/**
 * API client for the ZSWatch App Builder backend.
 */

import siteConfig from '@generated/docusaurus.config';

const API_BASE = typeof window !== 'undefined'
  ? (window.location.hostname === 'localhost'
    ? 'http://localhost:8000/api'
    : siteConfig.customFields.apiBaseUrl)
  : 'http://localhost:8000/api';

/**
 * Compile source files into a .llext binary.
 *
 * @param {Array<{name: string, content: string}>} files - Source files
 * @param {string} appName - App name (used for output filename)
 * @param {string|null} edkVersion - EDK version to use (null = latest)
 * @returns {Promise<{success: boolean, blob?: Blob, errors?: Array, raw_output?: string}>}
 */
export async function compileApp(files, appName = 'my_app', edkVersion = null) {
  const body = {
    files: files.map(f => ({ name: f.name, content: f.content })),
    app_name: appName,
    edk_version: edkVersion,
  };

  const res = await fetch(`${API_BASE}/compile`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(body),
  });

  if (!res.ok && res.status !== 200) {
    const text = await res.text();
    let detail;
    try {
      detail = JSON.parse(text).detail;
    } catch {
      detail = text;
    }
    return {
      success: false,
      errors: [{ severity: 'error', message: detail || `Server error: ${res.status}` }],
      raw_output: detail || '',
    };
  }

  const contentType = res.headers.get('Content-Type') || '';

  // If we got binary data back, it's the .llext
  if (contentType.includes('octet-stream')) {
    const blob = await res.blob();
    return { success: true, blob };
  }

  // Otherwise it's a JSON error response
  const json = await res.json();
  return {
    success: json.success || false,
    errors: json.errors || [],
    raw_output: json.raw_output || '',
  };
}

/**
 * Convert an image file to an LVGL v9 C array.
 *
 * @param {File} imageFile - Image file (PNG, JPG)
 * @param {string} format - Color format: RGB565, RGB565A8, ARGB8888, L8
 * @param {string|null} varName - C variable name (null = auto from filename)
 * @param {number|null} maxWidth - Max width (proportional resize)
 * @param {number|null} maxHeight - Max height (proportional resize)
 * @returns {Promise<{success: boolean, cSource?: string, error?: string}>}
 */
export async function convertImage(imageFile, format = 'RGB565A8', varName = null, maxWidth = null, maxHeight = null) {
  const formData = new FormData();
  formData.append('image', imageFile);
  formData.append('format', format);
  if (varName) formData.append('var_name', varName);
  if (maxWidth) formData.append('max_width', maxWidth.toString());
  if (maxHeight) formData.append('max_height', maxHeight.toString());

  const res = await fetch(`${API_BASE}/convert-image`, {
    method: 'POST',
    body: formData,
  });

  if (!res.ok) {
    const text = await res.text();
    let detail;
    try {
      detail = JSON.parse(text).detail;
    } catch {
      detail = text;
    }
    return { success: false, error: detail || `Server error: ${res.status}` };
  }

  const cSource = await res.text();
  return { success: true, cSource };
}

/**
 * Fetch available EDK versions.
 *
 * @returns {Promise<Array<{id: string, version: string}>>}
 */
export async function fetchEdkVersions() {
  try {
    const res = await fetch(`${API_BASE}/edk-versions`);
    if (!res.ok) return [];
    const json = await res.json();
    return json.versions || [];
  } catch {
    return [];
  }
}

/**
 * Health check.
 *
 * @returns {Promise<boolean>}
 */
export async function checkHealth() {
  try {
    const res = await fetch(`${API_BASE}/health`);
    return res.ok;
  } catch {
    return false;
  }
}
