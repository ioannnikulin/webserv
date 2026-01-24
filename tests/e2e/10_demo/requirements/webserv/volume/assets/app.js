/* ===============================
   BACKGROUND STARFIELD + 42 LOGOS
   =============================== */

   const mouse = {
	x: window.innerWidth / 2,
	y: window.innerHeight / 2
};


window.addEventListener("mousemove", (e) => {
	mouse.x = e.clientX;
	mouse.y = e.clientY;
});

   const starCanvas = document.getElementById("starfield");
   const starCtx = starCanvas.getContext("2d");
   
   let stars = [];
   let logos = [];
   
   /* ---- 42 logo image ---- */
   const LOGO_URL = "https://upload.wikimedia.org/wikipedia/commons/8/8d/42_Logo.svg";
   const logoImg = new Image();
   logoImg.crossOrigin = "anonymous";
   logoImg.src = LOGO_URL;
   
   /* ---- Resize ---- */
   function resizeStarfield() {
	   const dpr = window.devicePixelRatio || 1;
	   const width = window.innerWidth;
	   const height = window.innerHeight;
   
	   starCanvas.width = Math.floor(width * dpr);
	   starCanvas.height = Math.floor(height * dpr);
	   starCanvas.style.width = `${width}px`;
	   starCanvas.style.height = `${height}px`;
	   starCtx.setTransform(dpr, 0, 0, dpr, 0, 0);
   }
   
   /* ---- Stars ---- */
   function createStars(count) {
	   stars = Array.from({ length: count }, () => ({
		   x: Math.random() * window.innerWidth,
		   y: Math.random() * window.innerHeight,
		   r: Math.random() * 1.4 + 0.2,
		   vx: (Math.random() - 0.5) * 0.25,
		   vy: (Math.random() - 0.5) * 0.25,
		   alpha: Math.random() * 0.6 + 0.3
	   }));
   }
   
   /* ---- Logos ---- */
	/* ---- Logos (no overlap) ---- */
	function createLogos(count) {
		const LOGO_SIZE = 480;
		const MIN_DISTANCE = LOGO_SIZE + 60;
		const MAX_ATTEMPTS = 300;
	
		logos = [];
	
		for (let i = 0; i < count; i++) {
			let placed = false;
	
			for (let attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
				const x = Math.random() * window.innerWidth;
				const y = Math.random() * window.innerHeight;
	
				const overlaps = logos.some(l => {
					const dx = l.x - x;
					const dy = l.y - y;
					return Math.hypot(dx, dy) < MIN_DISTANCE;
				});
	
				if (!overlaps) {
					const depth = Math.random() * 0.7 + 0.3;
					const angle = Math.random() * Math.PI * 2;
					const speed = 0.8 * depth;
	
					logos.push({
						x,
						y,
	
						vx: Math.cos(angle) * speed,
						vy: Math.sin(angle) * speed,
	
						size: LOGO_SIZE * depth,
	
						rot: Math.random() * Math.PI * 2,
						vr: (Math.random() - 0.5) * 0.004 * depth,
	
						alpha: Math.random() * 0.02 + 0.02,
						depth
					});
	
					placed = true;
					break;
				}
			}
	
			if (!placed) {
				console.warn("Could not place logo", i);
			}
		}
	}
	

   
   /* ---- Update ---- */
   function updateStars() {
	   const w = window.innerWidth;
	   const h = window.innerHeight;
   
	   for (const s of stars) {
		   s.x += s.vx;
		   s.y += s.vy;
   
		   if (s.x < 0) s.x = w;
		   if (s.x > w) s.x = 0;
		   if (s.y < 0) s.y = h;
		   if (s.y > h) s.y = 0;
	   }
   
	   for (const l of logos) {
		// --- base movement ---
		l.x += l.vx;
		l.y += l.vy;
		l.rot += l.vr;
	
		// --- mouse interaction ---
		const dx = mouse.x - l.x;
		const dy = mouse.y - l.y;
		const dist = Math.hypot(dx, dy);
	
		if (dist < 300) {
			const force = (1 - dist / 300) * 0.02;
			l.vx += (dx / dist) * force;
			l.vy += (dy / dist) * force;
		}
	
		// --- soft space drift ---
		l.vx += Math.sin(l.y * 0.002) * 0.0005;
		l.vy += Math.cos(l.x * 0.002) * 0.0005;
	
		// --- damping (keeps things smooth) ---
		l.vx *= 0.995;
		l.vy *= 0.995;
	
		// --- wrap around screen ---
		if (l.x < -l.size) l.x = window.innerWidth + l.size;
		if (l.x > window.innerWidth + l.size) l.x = -l.size;
		if (l.y < -l.size) l.y = window.innerHeight + l.size;
		if (l.y > window.innerHeight + l.size) l.y = -l.size;
	}
   }
   
   /* ---- Draw ---- */
   function drawStars() {
	   starCtx.clearRect(0, 0, window.innerWidth, window.innerHeight);
   
	   // Stars
	   starCtx.fillStyle = "#ffffff";
	   for (const s of stars) {
		   starCtx.globalAlpha = s.alpha;
		   starCtx.beginPath();
		   starCtx.arc(s.x, s.y, s.r, 0, Math.PI * 2);
		   starCtx.fill();
	   }
   
	   // Logos
	   if (logoImg.complete && logoImg.naturalWidth) {
		   for (const l of logos) {
			   starCtx.save();
			   starCtx.globalAlpha = l.alpha;
			   starCtx.filter = "invert(1)";
			   starCtx.translate(l.x, l.y);
			   starCtx.rotate(l.rot);
			   starCtx.drawImage(
				   logoImg,
				   -l.size / 2,
				   -l.size / 2,
				   l.size,
				   l.size
			   );
			   starCtx.restore();
		   }
	   }
   
	   starCtx.globalAlpha = 1;
   }
   
   /* ---- Animate ---- */
   function animateStars() {
	   updateStars();
	   drawStars();
	   requestAnimationFrame(animateStars);
   }
   
   /* ---- Init ---- */
   resizeStarfield();
   createStars(120);
   createLogos(8);
   animateStars();
   

   window.addEventListener("resize", () => {
	   resizeStarfield();
	   createStars(120);
	   createLogos(8);
   });

   

const copyBlocks = document.querySelectorAll(".copy-block");
copyBlocks.forEach(block => {
	block.addEventListener("click", async () => {
		const text = block.innerText.trim();
		try {
			if (navigator.clipboard && navigator.clipboard.writeText) {
				await navigator.clipboard.writeText(text);
			} else {
				const selection = window.getSelection();
				const range = document.createRange();
				range.selectNodeContents(block);
				selection.removeAllRanges();
				selection.addRange(range);
				document.execCommand("copy");
				selection.removeAllRanges();
			}
			block.dataset.copied = "true";
			setTimeout(() => {
				delete block.dataset.copied;
			}, 800);
		} catch (err) {
			console.warn("Copy failed", err);
		}
	});
});

const fileInput = document.getElementById("rawFile");
const uploadBtn = document.getElementById("rawUploadBtn");
const uploadedFilesList = document.getElementById("uploadedFiles");
const dropZone = document.getElementById("rawDropZone");

const sections = document.querySelectorAll("details.section");
sections.forEach(section => {
	section.addEventListener("toggle", () => {
		if (!section.open) {
			return;
		}
		sections.forEach(other => {
			if (other !== section) {
				other.open = false;
			}
		});
		section.scrollIntoView({ behavior: "smooth", block: "start" });
	});
});

const deleteBtn = document.getElementById("deleteBtn");
const deleteInput = document.getElementById("deleteFileName");

let selectedFile = null;
let uploadedFiles = [];

/* ---------- Render ---------- */
function renderUploadedFiles() {
	uploadedFilesList.innerHTML = "";

	if (uploadedFiles.length === 0) {
		uploadedFilesList.innerHTML = "<li><em>No files uploaded yet</em></li>";
		return;
	}

	uploadedFiles.forEach(name => {
		const li = document.createElement("li");
		li.innerHTML = `
			${name}
			<button data-name="${name}" class="btn-danger">Delete</button>
		`;
		li.querySelector("button").addEventListener("click", () => {
			deleteFile(name);
		});
		uploadedFilesList.appendChild(li);
	});
}

/* ---------- Upload ---------- */
fileInput.addEventListener("change", (e) => {
	selectedFile = e.target.files[0];
	uploadBtn.disabled = !selectedFile;
});

dropZone.addEventListener("click", (event) => {
	if (event.target !== fileInput) {
		fileInput.click();
	}
});

dropZone.addEventListener("dragover", (event) => {
	event.preventDefault();
	dropZone.classList.add("is-dragging");
});

dropZone.addEventListener("dragleave", () => {
	dropZone.classList.remove("is-dragging");
});

dropZone.addEventListener("drop", (event) => {
	event.preventDefault();
	dropZone.classList.remove("is-dragging");

	const [file] = event.dataTransfer.files;
	if (!file) {
		return;
	}
	selectedFile = file;
	uploadBtn.disabled = false;

	try {
		const dataTransfer = new DataTransfer();
		dataTransfer.items.add(file);
		fileInput.files = dataTransfer.files;
	} catch (error) {
	}
});

uploadBtn.addEventListener("click", async () => {
	if (!selectedFile) return;

	uploadBtn.disabled = true;
	uploadBtn.textContent = "Uploading...";

	const res = await fetch("/folder/" + selectedFile.name, {
		method: "POST",
		headers: {
			"Content-Type": "application/octet-stream"
		},
		body: selectedFile
	});

	if (res.ok) {
		uploadedFiles.push(selectedFile.name);
		renderUploadedFiles();
		alert("Upload complete!");
		selectedFile = null;
		fileInput.value = "";
		uploadBtn.disabled = true;
	} else {
		alert("Upload failed: " + res.status);
	}

	uploadBtn.textContent = "Start raw upload";
	uploadBtn.disabled = false;
});

/* ---------- Delete ---------- */
async function deleteFile(filename) {
	const res = await fetch("/folder/" + filename, {
		method: "DELETE"
	});

	if (res.ok) {
		uploadedFiles = uploadedFiles.filter(f => f !== filename);
		renderUploadedFiles();
		alert("File deleted successfully!");
	} else {
		alert("Delete failed. Status: " + res.status);
	}
}

deleteBtn.addEventListener("click", async () => {
	const filename = deleteInput.value.trim();
	if (!filename) {
		alert("Please enter a filename to delete.");
		return;
	}

	await deleteFile(filename);
});
