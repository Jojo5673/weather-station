<template>
    <VContainer align="center" fluid>
        <VRow justify="center" style="max-width: 1200px;">
            <VCol align="center">
                <v-sheet class="mb-1 w-100 rounded-lg" style="max-width: 800;" color="transparent">
                    <v-card 
                        class="text-blue-grey-lighten-3" 
                        title="Unit Controls" 
                        color="#1e2d3d"
                        subtitle="Change display units on station" 
                        variant="flat" 
                        flat
                    ></v-card>
                </v-sheet>
                <v-sheet class="mb-1 w-100" style="max-width: 800;" color="transparent">
                    <v-card class="pa-5" color="#1e2d3d" variant="flat">
                        <v-radio-group v-model="units.temperature" label="Temperature Unit" inline>
                            <v-radio label="Celsius" value="c" color="blue-lighten-2"></v-radio>
                            <v-radio label="Fahrenheit" value="f" color="blue-lighten-2"></v-radio>
                        </v-radio-group>
                        <v-radio-group v-model="units.altitude" label="Altitude Unit" inline class="mt-4">
                            <v-radio label="Meters" value="m" color="blue-lighten-2"></v-radio>
                            <v-radio label="Feet" value="ft" color="blue-lighten-2"></v-radio>
                        </v-radio-group>
                        <v-radio-group v-model="units.pressure" label="Pressure Unit" inline class="mt-4">
                            <v-radio label="Pascals" value="pa" color="blue-lighten-2"></v-radio>
                            <v-radio label="Atmospheres" value="atm" color="blue-lighten-2"></v-radio>
                        </v-radio-group>
                        <v-btn class="mt-4" color="blue-darken-1" variant="tonal" @click="updateUnits">Update Units</v-btn>
                    </v-card>
                </v-sheet>
            </VCol>
        </VRow>
    </VContainer>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { reactive} from "vue";  
import { useAppStore } from '@/store/appStore'; // Import App Store



// VARIABLES

const units = reactive({"temperature": "c", "altitude": "m", "pressure": "pa"});
const AppStore = useAppStore();


// FUNCTIONS
const updateUnits = async () => {
    const result = await AppStore.updateUnits(units);
    if (result.status === 'success') {
        alert('Units updated successfully!');
    } else {
        alert('Failed to update units: ' + (result.message || 'Unknown error'));
    }
};


</script>


<style scoped>
:deep(.v-label) {
    color: #aac3e1 !important;
}

:deep(.v-radio .v-label) {
    color: #cdd9e8 !important;
}

:deep(.v-input__label) {
    color: #7a9cbf !important;
    font-weight: 600;
}
</style>
  