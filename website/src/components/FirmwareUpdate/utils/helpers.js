export const hashArrayToString = (hash) => {
  return Array.from(hash)
    .map((byte) => byte.toString(16).padStart(2, "0"))
    .join("");
};

export const formatFileSize = (bytes) => {
  return (bytes / 1024).toFixed(1) + " KB";
};

export const copyToClipboard = async (text, successMessage = "Copied to clipboard!") => {
  try {
    await navigator.clipboard.writeText(text);
    alert(successMessage);
  } catch (error) {
    console.error("Failed to copy to clipboard:", error);
  }
};
