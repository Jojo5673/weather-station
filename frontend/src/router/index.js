// Composables
import { createRouter, createWebHistory } from "vue-router";

const routes = [
  {
    path: "/",
    name: "Forecast",
    component: () => import("@/views/Forecast.vue"),
    meta: { transition: "fade" },
  },
  {
    path: "/control",
    name: "Control",
    component: () => import("@/views/Control.vue"),
    meta: { transition: "fade" },
  },
  {
    path: "/live",
    name: "Live",
    component: () => import("@/views/Live.vue"),
    meta: { transition: "fade" },
  },
  {
    path: "/analysis",
    name: "Analysis",
    component: () => import("@/views/Analysis.vue"),
    meta: { transition: "fade" },
  },
  // ADD NEW ROUTES HERE
];

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes,
});

export default router;